#include "RdnaDecoder/RdnaImageOpDecoder.hpp"
#include <stdexcept>

namespace ShaderRecompiler {

RdnaInstruction DecodeRdnaImageOp(std::span<const std::uint32_t> code, std::uint32_t wordIndex) {
    throw std::runtime_error("DecodeRdnaImageOp not implemented");
}

RdnaInstruction DecodeRdnaMimg(std::uint32_t programCounter, std::span<const std::uint32_t> code, std::uint32_t wordIndex) {
    throw std::runtime_error("DecodeRdnaMimg not implemented");
}

const char* GetRdnaImageSampleOpcodeName(std::uint32_t opcode) {
    throw std::runtime_error("GetRdnaImageSampleOpcodeName not implemented");
}

RdnaImageAddressComponent GetRdnaImageAddressComponentLayout(std::uint32_t flags, std::uint32_t component) {
    throw std::runtime_error("GetRdnaImageAddressComponentLayout not implemented");
}

std::uint32_t GetRdnaImageAddressDwordCount(std::uint32_t flags, std::uint32_t components) {
    throw std::runtime_error("GetRdnaImageAddressDwordCount not implemented");
}

}
