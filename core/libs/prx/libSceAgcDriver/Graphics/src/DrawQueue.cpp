#include "prx/libSceAgcDriver/Graphics/include/DrawQueue.hpp"
#include "prx/libSceAgcDriver/Execution/include/MemoryAccessScope.hpp"
#include "prx/libSceAgcDriver/Execution/include/PerformanceTimer.hpp"
#include <algorithm>

namespace AgcDriver::Graphics {

DrawQueue::~DrawQueue() {
    recording.commands.reset();
    for (auto& batch : pending) batch.commands.reset();
}

VkCommandBuffer DrawQueue::Begin(const Context& context) {
    if (drawCount >= 64) Wait();
    if (!recording.commands) {
        if (available.empty()) recording.commands = std::make_unique<CommandBatch>(context);
        else {
            recording.commands = std::move(available.back());
            available.pop_back();
            recording.commands->Reset();
        }
    }
    return recording.commands->Handle();
}

void DrawQueue::Enqueue(std::shared_ptr<ShaderResources> resources, std::shared_ptr<void> storage) {
    Require(recording.commands != nullptr && resources != nullptr && storage != nullptr, "draw batch is incomplete");
    recording.entries.push_back({std::move(storage), std::move(resources)});
    ++drawCount;
    if (recording.entries.size() >= 8) Flush();
}

void DrawQueue::Flush() {
    if (!recording.commands) return;
    Require(!recording.entries.empty(), "cannot submit an incomplete draw batch");
    pending.push_back(std::move(recording));
    recording = Batch{};
    pending.back().commands->Submit();
}

void DrawQueue::Resolve(std::uint64_t address, std::size_t bytes) {
    const auto overlaps = [&](const auto& entry) { return entry.resources->WritesOverlap(address, bytes); };
    if (std::any_of(recording.entries.begin(), recording.entries.end(), overlaps) || std::any_of(pending.begin(), pending.end(), [&](const auto& batch) { return std::any_of(batch.entries.begin(), batch.entries.end(), overlaps); })) Wait();
}

void DrawQueue::Wait() {
    if (pending.empty() && !recording.commands) return;
    PerformanceTimer timing("Graphics.DrawQueue.Wait");
    const GuestMemory::MemoryAccessScope suspended(nullptr, nullptr);
    Flush();
    timing.Mark("submit");
    for (auto& batch : pending) {
        batch.commands->Wait();
        timing.Mark("fence_wait");
        for (auto& entry : batch.entries) entry.resources->WriteBack();
        timing.Mark("resources_writeback");
        available.push_back(std::move(batch.commands));
        batch.entries.clear();
        timing.Mark("resources_release");
    }
    pending.clear();
    drawCount = 0;
}

}
