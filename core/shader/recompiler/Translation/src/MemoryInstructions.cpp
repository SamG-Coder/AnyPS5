#include "Translation/MemoryInstructions.hpp"
#include "Translation/TranslationContext.hpp"
#include <stdexcept>

namespace ShaderRecompiler {

void TranslateMemoryInstruction(IrBuilder& builder, const RdnaInstruction& instruction) {
    throw std::runtime_error("TranslateMemoryInstruction not implemented");
}

MemoryFlags TranslationContext::addMemoryInfo(const MemoryInfo& memory, std::uint32_t pc) {
    throw std::runtime_error("TranslationContext::addMemoryInfo not implemented");
}

TranslationContext::AddressOperands TranslationContext::readAddressOperands(const RdnaInstruction& inst, std::uint32_t firstSource) {
    throw std::runtime_error("TranslationContext::readAddressOperands not implemented");
}

IrU32 TranslationContext::getResourceDword(std::uint32_t index, std::uint32_t dword) {
    throw std::runtime_error("TranslationContext::getResourceDword not implemented");
}

IrValue* TranslationContext::getBufferResource(const MemoryInfo& memory) {
    throw std::runtime_error("TranslationContext::getBufferResource not implemented");
}

IrValue* TranslationContext::getAddressResource(IrValue* low, IrValue* high) {
    throw std::runtime_error("TranslationContext::getAddressResource not implemented");
}

IrValue* TranslationContext::getScalarAddressResource(std::uint32_t base) {
    throw std::runtime_error("TranslationContext::getScalarAddressResource not implemented");
}

IrValue* TranslationContext::getImageResource(const MemoryInfo& memory) {
    throw std::runtime_error("TranslationContext::getImageResource not implemented");
}

IrValue* TranslationContext::getSamplerResource(const MemoryInfo& memory) {
    throw std::runtime_error("TranslationContext::getSamplerResource not implemented");
}

IrValue* TranslationContext::makeImageAddress(const RdnaInstruction& inst, const RdnaOperand& base) {
    throw std::runtime_error("TranslationContext::makeImageAddress not implemented");
}

IrValue* TranslationContext::constructU32x4(const RdnaOperand& base, std::uint32_t count) {
    throw std::runtime_error("TranslationContext::constructU32x4 not implemented");
}

void TranslationContext::writeImageComponents(const RdnaOperand& dst, IrValue* value, const MemoryInfo& memory, std::uint32_t componentLimit) {
    throw std::runtime_error("TranslationContext::writeImageComponents not implemented");
}

TranslationContext::BufferAddress TranslationContext::readBufferAddress(const RdnaInstruction& inst, std::uint32_t sourceOffset) {
    throw std::runtime_error("TranslationContext::readBufferAddress not implemented");
}

IrU32 TranslationContext::widenSubdword(IrValue* value, std::uint32_t bits, bool sign) {
    throw std::runtime_error("TranslationContext::widenSubdword not implemented");
}

IrValue* TranslationContext::narrowSubdword(IrU32 value, std::uint32_t bits) {
    throw std::runtime_error("TranslationContext::narrowSubdword not implemented");
}

bool TranslationContext::sLoad(const RdnaInstruction& inst, bool raw) {
    throw std::runtime_error("TranslationContext::sLoad not implemented");
}

bool TranslationContext::bufferLoad(const RdnaInstruction& inst) {
    throw std::runtime_error("TranslationContext::bufferLoad not implemented");
}

bool TranslationContext::bufferStore(const RdnaInstruction& inst) {
    throw std::runtime_error("TranslationContext::bufferStore not implemented");
}

bool TranslationContext::bufferAtomic(const RdnaInstruction& inst, IrOpcode opcode) {
    throw std::runtime_error("TranslationContext::bufferAtomic not implemented");
}

bool TranslationContext::imageAtomic(const RdnaInstruction& inst, IrOpcode opcode) {
    throw std::runtime_error("TranslationContext::imageAtomic not implemented");
}

bool TranslationContext::dsAtomic(const RdnaInstruction& inst, IrOpcode opcode, bool returnsValue) {
    throw std::runtime_error("TranslationContext::dsAtomic not implemented");
}

bool TranslationContext::flatLoad(const RdnaInstruction& inst) {
    throw std::runtime_error("TranslationContext::flatLoad not implemented");
}

bool TranslationContext::flatStore(const RdnaInstruction& inst) {
    throw std::runtime_error("TranslationContext::flatStore not implemented");
}

bool TranslationContext::imageGetResinfo(const RdnaInstruction& inst) {
    throw std::runtime_error("TranslationContext::imageGetResinfo not implemented");
}

bool TranslationContext::imageGetLod(const RdnaInstruction& inst) {
    throw std::runtime_error("TranslationContext::imageGetLod not implemented");
}

bool TranslationContext::imageLoad(const RdnaInstruction& inst) {
    throw std::runtime_error("TranslationContext::imageLoad not implemented");
}

bool TranslationContext::imageStore(const RdnaInstruction& inst) {
    throw std::runtime_error("TranslationContext::imageStore not implemented");
}

bool TranslationContext::imageSample(const RdnaInstruction& inst) {
    throw std::runtime_error("TranslationContext::imageSample not implemented");
}

bool TranslationContext::imageGather(const RdnaInstruction& inst) {
    throw std::runtime_error("TranslationContext::imageGather not implemented");
}

IrValue* TranslationContext::loadSharedU32(std::uint32_t width, IrU32 address, const MemoryInfo& memory, std::uint32_t pc) {
    throw std::runtime_error("TranslationContext::loadSharedU32 not implemented");
}

IrValue* TranslationContext::extractSharedU32(IrValue* value, std::uint32_t width, std::uint32_t index) {
    throw std::runtime_error("TranslationContext::extractSharedU32 not implemented");
}

void TranslationContext::writeSharedU32(std::uint32_t width, IrU32 address, const std::array<IrValue*, 4>& values, const MemoryInfo& memory, std::uint32_t pc) {
    throw std::runtime_error("TranslationContext::writeSharedU32 not implemented");
}

bool TranslationContext::dsRead(const RdnaInstruction& inst) {
    throw std::runtime_error("TranslationContext::dsRead not implemented");
}

bool TranslationContext::dsRead2(const RdnaInstruction& inst) {
    throw std::runtime_error("TranslationContext::dsRead2 not implemented");
}

bool TranslationContext::dsWrite(const RdnaInstruction& inst) {
    throw std::runtime_error("TranslationContext::dsWrite not implemented");
}

bool TranslationContext::dsWrite2(const RdnaInstruction& inst) {
    throw std::runtime_error("TranslationContext::dsWrite2 not implemented");
}

bool TranslationContext::dsMinmaxF32(const RdnaInstruction& inst, IrOpcode opcode) {
    throw std::runtime_error("TranslationContext::dsMinmaxF32 not implemented");
}

bool TranslationContext::dsAppendConsume(const RdnaInstruction& inst, IrOpcode opcode) {
    throw std::runtime_error("TranslationContext::dsAppendConsume not implemented");
}

bool TranslationContext::dsAddtid(const RdnaInstruction& inst, bool write) {
    throw std::runtime_error("TranslationContext::dsAddtid not implemented");
}

bool TranslationContext::dsSwizzleB32(const RdnaInstruction& inst) {
    throw std::runtime_error("TranslationContext::dsSwizzleB32 not implemented");
}

bool TranslationContext::dsBpermuteB32(const RdnaInstruction& inst) {
    throw std::runtime_error("TranslationContext::dsBpermuteB32 not implemented");
}

bool TranslationContext::emitMemory(const RdnaInstruction& inst) {
    throw std::runtime_error("TranslationContext::emitMemory not implemented");
}

void TranslateMemoryInstruction(TranslationContext& context, const RdnaInstruction& instruction) {
    throw std::runtime_error("TranslateMemoryInstruction not implemented");
}

}
