#include "Translation/AttributeInstructions.hpp"
#include "Translation/TranslationContext.hpp"
#include <stdexcept>

namespace ShaderRecompiler {

void TranslateAttributeInstruction(IrBuilder& builder, const RdnaInstruction& instruction, const TranslateOptions& options) {
    throw std::runtime_error("TranslateAttributeInstruction not implemented");
}

ExportFlags TranslationContext::addExportInfo(const RdnaInstruction& inst) {
    throw std::runtime_error("TranslationContext::addExportInfo not implemented");
}

void TranslationContext::vInterpP1F32() {
    throw std::runtime_error("TranslationContext::vInterpP1F32 not implemented");
}

void TranslationContext::vInterpP2F32(const RdnaInstruction& inst) {
    throw std::runtime_error("TranslationContext::vInterpP2F32 not implemented");
}

void TranslationContext::vInterpMovF32(const RdnaInstruction& inst) {
    throw std::runtime_error("TranslationContext::vInterpMovF32 not implemented");
}

void TranslationContext::eXP(const RdnaInstruction& inst) {
    throw std::runtime_error("TranslationContext::eXP not implemented");
}

bool TranslationContext::emitInterpolation(const RdnaInstruction& inst) {
    throw std::runtime_error("TranslationContext::emitInterpolation not implemented");
}

void TranslationContext::TranslateEmbeddedFetch(const RdnaInstruction& instruction, std::uint32_t attribute, std::uint32_t componentCount, const ShaderBufferResource& resource) {
    throw std::runtime_error("TranslationContext::TranslateEmbeddedFetch not implemented");
}

void TranslateAttributeInstruction(TranslationContext& context, const RdnaInstruction& instruction) {
    throw std::runtime_error("TranslateAttributeInstruction not implemented");
}

}
