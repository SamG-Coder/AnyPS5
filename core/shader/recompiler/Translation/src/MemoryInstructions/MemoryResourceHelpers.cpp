#include "Translation/MemoryInstructions.hpp"
#include "Translation/TranslationContext.hpp"
#include <stdexcept>

namespace ShaderRecompiler {

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

}
