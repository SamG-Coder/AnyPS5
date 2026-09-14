#include "prx/libSceAgcDriver/Execution/include/Driver.hpp"
#include "prx/libSceAgcDriver/Execution/include/VulkanDevice.hpp"
#include <algorithm>
#include <array>
#include <condition_variable>
#include <cstring>
#include <deque>
#include <exception>
#include <limits>
#include <map>
#include <mutex>
#include <span>
#include <stdexcept>
#include <string>
#include <thread>
#include <vector>
#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#else
#include <fstream>
#include <sstream>
#endif

namespace AgcDriver {
namespace {

void require(bool condition, const char* reason) {
    if (!condition) {
        throw std::runtime_error(std::string("AGC driver: ") + reason);
    }
}

void checkRange(const void* pointer, std::size_t bytes, std::size_t alignment) {
    const auto address = reinterpret_cast<std::uintptr_t>(pointer);
    require(address != 0 && address % alignment == 0, "null or misaligned address");
    require(bytes <= std::numeric_limits<std::uintptr_t>::max() - address, "address range overflow");
    auto cursor = address;
    const auto end = address + bytes;
#ifdef _WIN32
    while (cursor < end) {
        MEMORY_BASIC_INFORMATION memory{};
        require(VirtualQuery(reinterpret_cast<const void*>(cursor), &memory, sizeof(memory)) == sizeof(memory), "cannot query guest memory");
        require(memory.State == MEM_COMMIT && (memory.Protect & (PAGE_GUARD | PAGE_NOACCESS)) == 0, "guest memory is not readable");
        const auto protection = memory.Protect & 0xffu;
        require(protection == PAGE_READONLY || protection == PAGE_READWRITE || protection == PAGE_WRITECOPY || protection == PAGE_EXECUTE_READ || protection == PAGE_EXECUTE_READWRITE || protection == PAGE_EXECUTE_WRITECOPY, "guest memory has no read permission");
        const auto base = reinterpret_cast<std::uintptr_t>(memory.BaseAddress);
        require(memory.RegionSize <= std::numeric_limits<std::uintptr_t>::max() - base && base + memory.RegionSize > cursor, "invalid guest memory mapping");
        cursor = std::min(end, base + memory.RegionSize);
    }
#else
    std::ifstream maps("/proc/self/maps");
    require(maps.is_open(), "cannot query guest memory maps");
    std::string line;
    while (cursor < end && std::getline(maps, line)) {
        std::istringstream fields(line);
        std::uintptr_t first = 0;
        std::uintptr_t last = 0;
        char separator = 0;
        std::string permissions;
        require(static_cast<bool>(fields >> std::hex >> first >> separator >> last >> permissions) && separator == '-' && first < last && !permissions.empty(), "invalid guest memory map entry");
        if (last <= cursor) continue;
        require(first <= cursor && permissions[0] == 'r', "guest memory is not readable");
        cursor = std::min(end, last);
    }
    require(cursor == end, "guest address range is not mapped");
#endif
}

using Registers = std::map<std::uint32_t, std::uint32_t>;

struct QueueState {
    Registers shader;
    Registers context;
    Registers userConfig;
};

struct ShaderSnapshot {
    std::uint64_t codeAddress;
    std::uint64_t headerAddress;
    std::uint8_t type;
    std::vector<std::uint32_t> code;
    std::vector<std::byte> header;
};

struct Submission {
    std::uint64_t serial;
    std::uint32_t queue;
    std::vector<std::uint32_t> commands;
    std::map<std::uint64_t, std::shared_ptr<const ShaderSnapshot>> shaders;
};

std::uint32_t readRegister(const Registers& registers, std::uint32_t offset) {
    const auto it = registers.find(offset);
    require(it != registers.end(), "required shader register has not been written");
    return it->second;
}

std::vector<ShaderRecompiler::RegisterValue> registerValues(const Registers& registers) {
    std::vector<ShaderRecompiler::RegisterValue> result;
    result.reserve(registers.size());
    for (auto [offset, value] : registers) {
        result.push_back({offset, value});
    }
    return result;
}

class Driver {
public:
    static Driver& Get() {
        static Driver driver;
        return driver;
    }

    ~Driver() {
        {
            std::lock_guard lock(mutex);
            stopping = true;
        }
        changed.notify_all();
        worker.join();
    }

    void Submit(const Packet* packet, std::uint32_t queue) {
        CheckFailure();
        require(queue == 0 || (queue >= 0x20 && queue < 0x58), "unsupported compute queue");
        checkRange(packet, sizeof(Packet), alignof(Packet));
        const auto descriptor = *packet;
        require(descriptor.flags == 0, "nonzero submission flags are not implemented");
        Submission submission{};
        submission.queue = queue;
        if (descriptor.dw_num != 0) {
            require(descriptor.dw_num <= std::numeric_limits<std::size_t>::max() / sizeof(std::uint32_t), "command size overflow");
            checkRange(descriptor.addr, static_cast<std::size_t>(descriptor.dw_num) * sizeof(std::uint32_t), alignof(std::uint32_t));
            submission.commands.assign(descriptor.addr, descriptor.addr + descriptor.dw_num);
        }
        validate(submission.commands, queue);
        {
            std::lock_guard lock(mutex);
            rethrowFailure();
            require(!stopping, "submission during shutdown");
            require(accepted != std::numeric_limits<std::uint64_t>::max(), "submission serial overflow");
            submission.shaders = shaders;
            submission.serial = accepted + 1;
            pending.push_back(std::move(submission));
            ++accepted;
        }
        changed.notify_all();
    }

    void WaitIdle() {
        require(std::this_thread::get_id() != worker.get_id(), "worker cannot wait for itself");
        std::unique_lock lock(mutex);
        const auto target = accepted;
        changed.wait(lock, [&] { return failure != nullptr || completed >= target; });
        rethrowFailure();
    }

    void CheckFailure() {
        std::lock_guard lock(mutex);
        rethrowFailure();
    }

    void RegisterShader(const Shader* shader) {
        CheckFailure();
        checkRange(shader, sizeof(Shader), alignof(Shader));
        require(shader->file_header == 0x34333231u && shader->version == 0x18u, "invalid shader header");
        require(shader->header_size >= sizeof(Shader), "shader header is smaller than its fixed fields");
        require(shader->shader_size != 0 && (shader->shader_size & 3u) == 0, "invalid shader size");
        checkRange(shader, shader->header_size, alignof(Shader));
        const auto* code = const_cast<const void*>(shader->code);
        checkRange(code, shader->shader_size, 256);
        ShaderSnapshot snapshot{reinterpret_cast<std::uintptr_t>(code), reinterpret_cast<std::uintptr_t>(shader), shader->type, {}, {}};
        snapshot.code.resize(shader->shader_size / sizeof(std::uint32_t));
        std::memcpy(snapshot.code.data(), code, shader->shader_size);
        snapshot.header.resize(shader->header_size);
        std::memcpy(snapshot.header.data(), shader, shader->header_size);
        std::lock_guard lock(mutex);
        rethrowFailure();
        const auto address = snapshot.codeAddress;
        shaders.insert_or_assign(address, std::make_shared<const ShaderSnapshot>(std::move(snapshot)));
    }

private:
    std::mutex mutex;
    std::condition_variable changed;
    std::deque<Submission> pending;
    std::map<std::uint64_t, std::shared_ptr<const ShaderSnapshot>> shaders;
    std::map<std::uint32_t, QueueState> queues;
    std::unique_ptr<VulkanDevice> device;
    std::uint64_t accepted = 0;
    std::uint64_t completed = 0;
    std::exception_ptr failure;
    bool stopping = false;
    std::thread worker;

    Driver() : worker([this] { run(); }) {}

    void rethrowFailure() const {
        if (failure != nullptr) {
            std::rethrow_exception(failure);
        }
    }

    static void validate(std::span<const std::uint32_t> commands, std::uint32_t queue) {
        for (std::size_t cursor = 0; cursor < commands.size();) {
            const auto header = commands[cursor];
            require((header & 0xc0000000u) == 0xc0000000u, "unsupported PM4 packet type");
            const auto count = static_cast<std::size_t>((header >> 16u) & 0x3fffu) + 2;
            require(count <= commands.size() - cursor, "truncated PM4 packet");
            require((header & 0xffu) == 0, "PM4 header flags or internal extension are not implemented");
            const auto packet = commands.subspan(cursor, count);
            const auto opcode = (header >> 8u) & 0xffu;
            switch (opcode) {
                case 0x10:
                    require((packet[1] & 0xffff0000u) != 0x68750000u, "marker NOP is not implemented");
                    break;
                case 0x69:
                case 0x76:
                case 0x79:
                    require(count >= 3, "register packet has no values");
                    require(packet[1] <= 0xffffu && count - 2 <= 0x10000u - packet[1], "register range overflow or unsupported register index");
                    require(queue == 0 || opcode == 0x76, "graphics register packet in compute queue");
                    break;
                case 0x15:
                    require(count == 5, "invalid dispatch packet size");
                    require((packet[4] & ~0x8000u) == 0x41u, "dispatch modifiers are not implemented");
                    break;
                default:
                    throw std::runtime_error("AGC driver: unsupported PM4 opcode " + std::to_string(opcode) + " at DWORD " + std::to_string(cursor));
            }
            cursor += count;
        }
    }

    void dispatch(QueueState& queue, std::span<const std::uint32_t> packet, const Submission& submission) {
        const auto address = (static_cast<std::uint64_t>(readRegister(queue.shader, 0x20c)) << 8u) | (static_cast<std::uint64_t>(readRegister(queue.shader, 0x20d) & 0xffu) << 40u);
        auto it = submission.shaders.upper_bound(address);
        require(it != submission.shaders.begin(), "compute program does not belong to a registered shader");
        --it;
        const auto& snapshot = *it->second;
        require(address - snapshot.codeAddress < snapshot.code.size() * sizeof(std::uint32_t), "compute program is outside registered shader code");
        require(snapshot.type == 0, "compute program refers to a non-compute shader");
        const auto userCount = (readRegister(queue.shader, 0x213) >> 1u) & 0x1fu;
        std::vector<std::uint32_t> userData;
        for (std::uint32_t i = 0; i < userCount; ++i) {
            userData.push_back(readRegister(queue.shader, 0x240 + i));
        }
        auto shaderRegisters = registerValues(queue.shader);
        auto contextRegisters = registerValues(queue.context);
        auto userConfigRegisters = registerValues(queue.userConfig);
        const std::array<ShaderRecompiler::MemoryRegion, 2> memory{{{snapshot.codeAddress, std::as_bytes(std::span(snapshot.code))}, {snapshot.headerAddress, snapshot.header}}};
        if (device == nullptr) {
            device = std::make_unique<VulkanDevice>();
        }
        const auto codeOffset = static_cast<std::size_t>((address - snapshot.codeAddress) / sizeof(std::uint32_t));
        const ShaderRecompiler::RecompileRequest request{
            {ShaderRecompiler::ShaderStage::Compute, address, std::span(snapshot.code).subspan(codeOffset), snapshot.headerAddress, snapshot.header},
            {(packet[4] & 0x8000u) != 0 ? 32u : 64u, 0x240, userData, shaderRegisters, contextRegisters, userConfigRegisters, memory},
            device->Target(),
            {0, 0, 0, 128}
        };
        const auto compiled = ShaderRecompiler::Recompile(request);
        device->Dispatch(compiled, packet[1], packet[2], packet[3]);
    }

    void execute(const Submission& submission) {
        auto& queue = queues[submission.queue];
        for (std::size_t cursor = 0; cursor < submission.commands.size();) {
            const auto header = submission.commands[cursor];
            const auto count = static_cast<std::size_t>((header >> 16u) & 0x3fffu) + 2;
            const auto packet = std::span(submission.commands).subspan(cursor, count);
            switch ((header >> 8u) & 0xffu) {
                case 0x10:
                    break;
                case 0x69:
                case 0x76:
                case 0x79: {
                    const auto opcode = (header >> 8u) & 0xffu;
                    auto& registers = opcode == 0x69 ? queue.context : opcode == 0x76 ? queue.shader : queue.userConfig;
                    for (std::size_t i = 2; i < count; ++i) {
                        registers.insert_or_assign(packet[1] + static_cast<std::uint32_t>(i - 2), packet[i]);
                    }
                    break;
                }
                case 0x15:
                    dispatch(queue, packet, submission);
                    break;
                default:
                    throw std::runtime_error("AGC driver: validated packet has no executor");
            }
            cursor += count;
        }
    }

    void run() noexcept {
        try {
            for (;;) {
                Submission submission;
                {
                    std::unique_lock lock(mutex);
                    changed.wait(lock, [&] { return stopping || !pending.empty(); });
                    if (pending.empty()) {
                        break;
                    }
                    submission = std::move(pending.front());
                    pending.pop_front();
                }
                execute(submission);
                {
                    std::lock_guard lock(mutex);
                    completed = submission.serial;
                }
                changed.notify_all();
            }
            device.reset();
        } catch (...) {
            const auto error = std::current_exception();
            device.reset();
            {
                std::lock_guard lock(mutex);
                failure = error;
                if (failure == nullptr) {
                    std::terminate();
                }
                pending.clear();
            }
            changed.notify_all();
        }
    }
};

}

void Submit(const Packet* packet, std::uint32_t queue) {
    Driver::Get().Submit(packet, queue);
}

void WaitIdle() {
    Driver::Get().WaitIdle();
}

void RegisterShader(const Shader* shader) {
    Driver::Get().RegisterShader(shader);
}

}

extern "C" void AgcDriverWaitIdle_nid_postfix() {
    AgcDriver::WaitIdle();
}

extern "C" void AgcDriverRegisterShader_nid_postfix(const Shader* shader) {
    AgcDriver::RegisterShader(shader);
}
