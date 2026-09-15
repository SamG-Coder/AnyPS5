#include "prx/libSceAgcDriver/Graphics/include/Pipeline.hpp"
#include <spirv/unified1/spirv.hpp>
#include <algorithm>
#include <map>
#include <optional>
#include <set>
#include <string>
#include <vector>

namespace AgcDriver::Graphics {
namespace {

struct Decoration {
    std::optional<std::uint32_t> location;
    std::optional<std::uint32_t> builtin;
    std::optional<std::uint32_t> set;
    std::optional<std::uint32_t> binding;
    std::optional<std::uint32_t> stride;
    bool bufferBlock = false;
};

struct Variable {
    std::uint32_t pointer;
    std::uint32_t storage;
};

struct Module {
    std::map<std::uint32_t, std::vector<std::uint32_t>> types;
    std::map<std::uint32_t, Decoration> decorations;
    std::map<std::pair<std::uint32_t, std::uint32_t>, std::uint32_t> builtins;
    std::map<std::pair<std::uint32_t, std::uint32_t>, std::uint32_t> offsets;
    std::map<std::uint32_t, Variable> variables;
    std::map<std::uint32_t, std::uint32_t> constants;
    std::set<std::uint32_t> interface;
    std::map<std::uint32_t, std::string> inputs;
    std::map<std::uint32_t, std::string> outputs;
    bool position = false;

    const std::vector<std::uint32_t>& Type(std::uint32_t id) const {
        const auto it = types.find(id);
        Require(it != types.end(), "SPIR-V refers to an unknown type");
        return it->second;
    }

    std::string Signature(std::uint32_t id, std::uint32_t depth = 0) const {
        Require(depth < 8, "SPIR-V interface type nesting exceeds supported depth");
        const auto& type = Type(id);
        const auto op = static_cast<spv::Op>(type[0] & 0xffffu);
        if (op == spv::OpTypeFloat && type.size() == 3 && type[2] == 32) return "f32";
        if (op == spv::OpTypeInt && type.size() == 4 && type[2] == 32 && type[3] <= 1) return type[3] != 0 ? "i32" : "u32";
        if (op == spv::OpTypeBool && type.size() == 2) return "bool";
        if (op == spv::OpTypeVector && type.size() == 4 && type[3] >= 2 && type[3] <= 4) return Signature(type[2], depth + 1) + "x" + std::to_string(type[3]);
        throw std::runtime_error("AGC graphics: unsupported SPIR-V interface type");
    }

    std::uint64_t Size(std::uint32_t id, std::uint32_t depth = 0) {
        Require(depth < 8, "SPIR-V push constant type nesting exceeds supported depth");
        const auto& type = Type(id);
        const auto op = static_cast<spv::Op>(type[0] & 0xffffu);
        if ((op == spv::OpTypeInt || op == spv::OpTypeFloat) && type.size() >= 3 && type[2] == 32) return 4;
        if (op == spv::OpTypeVector && type.size() == 4 && type[3] >= 2 && type[3] <= 4) return Size(type[2], depth + 1) * type[3];
        if (op == spv::OpTypeArray && type.size() == 4) {
            const auto count = constants.find(type[3]);
            const auto stride = decorations[id].stride;
            Require(count != constants.end() && count->second != 0 && count->second <= StagePushConstantBytes && stride.has_value(), "invalid push constant array");
            const auto elementSize = Size(type[2], depth + 1);
            Require(*stride >= elementSize && *stride <= StagePushConstantBytes, "invalid push constant array stride");
            return static_cast<std::uint64_t>(count->second - 1) * *stride + elementSize;
        }
        throw std::runtime_error("AGC graphics: unsupported push constant member type");
    }

    void Builtin(std::uint32_t value, std::uint32_t type, std::uint32_t storage, bool vertex) {
        const auto signature = Signature(type);
        if (vertex && storage == spv::StorageClassInput) {
            Require((value == spv::BuiltInVertexIndex || value == spv::BuiltInInstanceIndex) && signature == "i32", "unsupported vertex built-in input");
        } else if (vertex && storage == spv::StorageClassOutput) {
            Require(value == spv::BuiltInPosition && signature == "f32x4" && !position, "unsupported or duplicate vertex built-in output");
            position = true;
        } else {
            Require(storage == spv::StorageClassInput && ((value == spv::BuiltInFragCoord && signature == "f32x4") || (value == spv::BuiltInFrontFacing && signature == "bool")), "unsupported fragment built-in");
        }
    }
};

Module inspect(const ShaderRecompiler::RecompileResult& shader, bool vertex) {
    const auto& words = shader.spirv;
    Require(words.size() >= 5 && words[0] == spv::MagicNumber && words[1] >= 0x10000u && words[1] <= 0x10300u && words[3] != 0 && words[4] == 0, "invalid or unsupported SPIR-V header");
    Module module;
    std::uint32_t entries = 0;
    std::uint32_t memoryModels = 0;
    bool upperLeft = false;
    for (std::size_t cursor = 5; cursor < words.size();) {
        const auto count = words[cursor] >> 16u;
        Require(count != 0 && count <= words.size() - cursor, "truncated SPIR-V instruction");
        const auto op = static_cast<spv::Op>(words[cursor] & 0xffffu);
        const auto instruction = std::span(words).subspan(cursor, count);
        switch (op) {
            case spv::OpCapability:
                Require(count == 2 && instruction[1] == spv::CapabilityShader, "SPIR-V requires an unsupported device capability");
                break;
            case spv::OpExtension:
            case spv::OpDecorateId:
            case spv::OpDecorationGroup:
            case spv::OpGroupDecorate:
            case spv::OpGroupMemberDecorate:
            case spv::OpSpecConstant:
            case spv::OpSpecConstantTrue:
            case spv::OpSpecConstantFalse:
            case spv::OpSpecConstantComposite:
            case spv::OpSpecConstantOp:
                throw std::runtime_error("AGC graphics: unsupported SPIR-V extension, grouped decoration or specialization constant");
            case spv::OpMemoryModel:
                Require(count == 3 && instruction[1] == spv::AddressingModelLogical && instruction[2] == spv::MemoryModelGLSL450, "unsupported SPIR-V memory model");
                ++memoryModels;
                break;
            case spv::OpEntryPoint:
                Require(count >= 5 && instruction[1] == (vertex ? spv::ExecutionModelVertex : spv::ExecutionModelFragment) && instruction[3] == 0x6e69616du && instruction[4] == 0, "expected a main entry point for the assigned graphics stage");
                ++entries;
                for (std::size_t i = 5; i < count; ++i) Require(module.interface.insert(instruction[i]).second, "duplicate SPIR-V interface ID");
                break;
            case spv::OpExecutionMode:
                Require(!vertex && count == 3 && (instruction[2] == spv::ExecutionModeOriginUpperLeft || instruction[2] == spv::ExecutionModeEarlyFragmentTests), "unsupported graphics shader execution mode");
                if (instruction[2] == spv::ExecutionModeOriginUpperLeft) upperLeft = true;
                break;
            case spv::OpDecorate: {
                Require(count >= 3, "malformed SPIR-V decoration");
                auto& decoration = module.decorations[instruction[1]];
                const auto kind = static_cast<spv::Decoration>(instruction[2]);
                if (kind == spv::DecorationLocation || kind == spv::DecorationBuiltIn || kind == spv::DecorationDescriptorSet || kind == spv::DecorationBinding || kind == spv::DecorationArrayStride) {
                    Require(count == 4, "malformed SPIR-V literal decoration");
                    auto* field = kind == spv::DecorationLocation ? &decoration.location : kind == spv::DecorationBuiltIn ? &decoration.builtin : kind == spv::DecorationDescriptorSet ? &decoration.set : kind == spv::DecorationBinding ? &decoration.binding : &decoration.stride;
                    Require(!field->has_value(), "duplicate SPIR-V decoration");
                    *field = instruction[3];
                }
                Require(kind != spv::DecorationComponent && kind != spv::DecorationIndex && kind != spv::DecorationStream && kind != spv::DecorationXfbBuffer && kind != spv::DecorationXfbStride, "unsupported shader interface packing or transform feedback");
                if (kind == spv::DecorationBufferBlock) decoration.bufferBlock = true;
                break;
            }
            case spv::OpMemberDecorate:
                Require(count >= 4, "malformed SPIR-V member decoration");
                if (instruction[3] == spv::DecorationBuiltIn || instruction[3] == spv::DecorationOffset) {
                    Require(count == 5, "malformed SPIR-V member literal decoration");
                    auto& fields = instruction[3] == spv::DecorationBuiltIn ? module.builtins : module.offsets;
                    Require(fields.emplace(std::make_pair(instruction[1], instruction[2]), instruction[4]).second, "duplicate SPIR-V member decoration");
                }
                break;
            case spv::OpTypeBool:
            case spv::OpTypeInt:
            case spv::OpTypeFloat:
            case spv::OpTypeVector:
            case spv::OpTypeMatrix:
            case spv::OpTypeArray:
            case spv::OpTypeRuntimeArray:
            case spv::OpTypeStruct:
            case spv::OpTypePointer:
                Require(count >= 2 && module.types.emplace(instruction[1], std::vector<std::uint32_t>(instruction.begin(), instruction.end())).second, "invalid or duplicate SPIR-V type");
                break;
            case spv::OpVariable:
                Require(count >= 4 && module.variables.emplace(instruction[2], Variable{instruction[1], instruction[3]}).second, "invalid or duplicate SPIR-V variable");
                break;
            case spv::OpConstant:
                if (count == 4) module.constants.emplace(instruction[2], instruction[3]);
                break;
            default: break;
        }
        cursor += count;
    }
    Require(entries == 1 && memoryModels == 1, "SPIR-V must contain one entry point and memory model");
    Require(vertex || upperLeft, "fragment coordinates must use an upper-left origin");
    std::set<std::pair<std::uint32_t, std::uint32_t>> descriptors;
    bool push = false;
    for (const auto& [id, variable] : module.variables) {
        if (variable.storage == spv::StorageClassFunction || variable.storage == spv::StorageClassPrivate) continue;
        const auto& pointer = module.Type(variable.pointer);
        Require(pointer.size() == 4 && (pointer[0] & 0xffffu) == spv::OpTypePointer && pointer[2] == variable.storage, "invalid SPIR-V variable pointer");
        const auto typeId = pointer[3];
        const auto& type = module.Type(typeId);
        const auto& decoration = module.decorations[id];
        if (variable.storage == spv::StorageClassInput || variable.storage == spv::StorageClassOutput) {
            Require(module.interface.contains(id), "SPIR-V input or output is absent from the entry point interface");
            if (decoration.location) {
                Require(!decoration.builtin && !(vertex && variable.storage == spv::StorageClassInput), "vertex attributes require unsupported vertex input bindings");
                auto& locations = variable.storage == spv::StorageClassInput ? module.inputs : module.outputs;
                Require(locations.emplace(*decoration.location, module.Signature(typeId)).second, "duplicate shader interface location");
            } else if (decoration.builtin) {
                module.Builtin(*decoration.builtin, typeId, variable.storage, vertex);
            } else {
                Require((type[0] & 0xffffu) == spv::OpTypeStruct, "shader interface lacks a location or built-in");
                for (std::size_t i = 2; i < type.size(); ++i) {
                    const auto builtin = module.builtins.find({typeId, static_cast<std::uint32_t>(i - 2)});
                    Require(builtin != module.builtins.end(), "interface blocks with non-built-in members are unsupported");
                    module.Builtin(builtin->second, type[i], variable.storage, vertex);
                }
            }
        } else if (variable.storage == spv::StorageClassPushConstant) {
            Require(!push && !shader.pushConstants.empty() && (type[0] & 0xffffu) == spv::OpTypeStruct, "invalid push constant interface");
            push = true;
            const auto first = vertex ? 0u : StagePushConstantBytes;
            for (std::size_t i = 2; i < type.size(); ++i) {
                const auto offset = module.offsets.find({typeId, static_cast<std::uint32_t>(i - 2)});
                Require(offset != module.offsets.end(), "push constant member has no offset");
                Require(offset->second >= first && static_cast<std::uint64_t>(offset->second) + module.Size(type[i]) <= first + shader.pushConstants.size(), "push constant member exceeds its stage range");
            }
        } else {
            Require((variable.storage == spv::StorageClassUniform || variable.storage == spv::StorageClassStorageBuffer) && decoration.set && decoration.binding, "unsupported or unbound shader resource");
            Require((type[0] & 0xffffu) == spv::OpTypeStruct, "descriptor arrays and non-buffer shader resources are unsupported");
            const auto key = std::make_pair(*decoration.set, *decoration.binding);
            Require(descriptors.insert(key).second, "duplicate SPIR-V resource binding");
            const auto binding = std::find_if(shader.bindings.begin(), shader.bindings.end(), [&](const auto& item) { return item.descriptorSet == key.first && item.binding == key.second; });
            Require(binding != shader.bindings.end(), "SPIR-V resource is absent from recompiler binding metadata");
            const auto kind = variable.storage == spv::StorageClassStorageBuffer || module.decorations[typeId].bufferBlock ? ShaderRecompiler::DescriptorKind::StorageBuffer : ShaderRecompiler::DescriptorKind::UniformBuffer;
            Require(binding->kind == kind, "SPIR-V descriptor type disagrees with recompiler binding metadata");
        }
    }
    Require(descriptors.size() == shader.bindings.size(), "recompiler binding metadata contains undeclared resources");
    Require(push == !shader.pushConstants.empty(), "recompiler push constant metadata disagrees with SPIR-V");
    for (const auto id : module.interface) Require(module.variables.contains(id), "entry point interface contains an unknown variable");
    if (vertex) Require(module.position, "vertex shader does not export position");
    return module;
}

}

void ValidateShaderPair(const ShaderRecompiler::RecompileResult& vertex, const ShaderRecompiler::RecompileResult& fragment) {
    const auto vs = inspect(vertex, true);
    const auto ps = inspect(fragment, false);
    Require(ps.outputs.size() == 1 && ps.outputs.contains(0) && ps.outputs.at(0) == "f32x4", "fragment shader must export one float4 color at location zero");
    for (const auto& [location, signature] : ps.inputs) {
        const auto output = vs.outputs.find(location);
        Require(output != vs.outputs.end() && output->second == signature, "vertex and fragment interfaces disagree at location " + std::to_string(location));
    }
}

}
