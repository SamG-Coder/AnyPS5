#include "prx/libSceAgcDriver/Graphics/include/DrawQueue.hpp"
#include "prx/libSceAgcDriver/Execution/include/MemoryAccessScope.hpp"
#include "prx/libSceAgcDriver/Execution/include/PerformanceTimer.hpp"
#include <algorithm>

namespace AgcDriver::Graphics {

DrawQueue::~DrawQueue() {
    for (auto& entry : pending) entry.commands.reset();
}

void DrawQueue::Submit(std::unique_ptr<CommandBatch> commands, std::shared_ptr<ShaderResources> resources, std::shared_ptr<void> storage) {
    if (pending.size() >= 64) Wait();
    pending.push_back({std::move(storage), std::move(resources), std::move(commands)});
    try {
        pending.back().commands->Submit();
    } catch (...) {
        pending.pop_back();
        throw;
    }
}

void DrawQueue::Resolve(std::uint64_t address, std::size_t bytes) {
    if (std::any_of(pending.begin(), pending.end(), [&](const auto& entry) { return entry.resources->WritesOverlap(address, bytes); })) Wait();
}

void DrawQueue::Wait() {
    if (pending.empty()) return;
    PerformanceTimer timing("Graphics.DrawQueue.Wait");
    const GuestMemory::MemoryAccessScope suspended(nullptr, nullptr);
    for (auto& entry : pending) {
        entry.commands->Wait();
        timing.Mark("fence_wait");
        entry.resources->WriteBack();
        timing.Mark("resources_writeback");
    }
    pending.clear();
}

}
