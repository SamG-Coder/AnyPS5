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
    bool patch = false;
    bool perPrimitive = false;
    bool nonWritable = false;
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
    std::set<std::pair<std::uint32_t, std::uint32_t>> readOnlyMembers;
    std::map<std::uint32_t, std::uint32_t> constants;
    std::set<std::uint32_t> interface;
    std::map<std::uint32_t, std::string> inputs;
    std::map<std::uint32_t, std::string> outputs;
    bool position = false;
    bool primitiveIndices = false;
    std::map<std::uint32_t, std::vector<std::uint32_t>> modes;

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

    std::uint32_t AddLocations(std::map<std::uint32_t, std::string>& locations, std::uint32_t location, std::uint32_t typeId, bool patch, std::uint32_t depth = 0) const {
        Require(depth < 8 && location < 256, "graphics interface exceeds supported locations");
        const auto& type = Type(typeId);
        if ((type[0] & 0xffffu) == spv::OpTypeArray) {
            Require(type.size() == 4, "malformed interface array");
            const auto count = constants.find(type[3]);
            Require(count != constants.end() && count->second != 0 && count->second <= 256, "invalid interface array length");
            for (std::uint32_t i = 0; i < count->second; ++i) location = AddLocations(locations, location, type[2], patch, depth + 1);
            return location;
        }
        Require(locations.emplace(location | (patch ? 0x10000u : 0u), std::string(patch ? "patch:" : "vertex:") + Signature(typeId)).second, "duplicate shader interface location");
        return location + 1;
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

    void Builtin(std::uint32_t value, std::uint32_t type, std::uint32_t storage, ShaderRecompiler::ShaderStage stage) {
        using Stage = ShaderRecompiler::ShaderStage;
        const bool vertex = stage == Stage::Vertex || stage == Stage::Local;
        const bool input = storage == spv::StorageClassInput;
        const auto& raw = Type(type);
        if ((value == spv::BuiltInTessLevelOuter || value == spv::BuiltInTessLevelInner) && (stage == Stage::TessellationControl || stage == Stage::TessellationEvaluation)) {
            Require(raw.size() == 4 && (raw[0] & 0xffffu) == spv::OpTypeArray && Signature(raw[2]) == "f32", "invalid tessellation level type");
            const auto count = constants.find(raw[3]);
            Require(count != constants.end() && count->second == (value == spv::BuiltInTessLevelOuter ? 4u : 2u), "invalid tessellation level count");
            Require(input == (stage == Stage::TessellationEvaluation), "invalid tessellation level direction");
            return;
        }
        if (stage == Stage::TessellationControl || stage == Stage::TessellationEvaluation || stage == Stage::Mesh) {
            const auto signature = Signature(type);
            if (!input && value == spv::BuiltInPosition) {
                Require(signature == "f32x4" && !position, "invalid or duplicate position output");
                position = true;
                return;
            }
            if (stage == Stage::TessellationControl || stage == Stage::TessellationEvaluation) {
                Require(input && ((value == spv::BuiltInPosition && signature == "f32x4") || (value == spv::BuiltInTessCoord && stage == Stage::TessellationEvaluation && signature == "f32x3") || ((value == spv::BuiltInInvocationId || value == spv::BuiltInPrimitiveId || value == spv::BuiltInPatchVertices) && (signature == "i32" || signature == "u32"))), "unsupported tessellation built-in");
                return;
            }
            if (!input && value == spv::BuiltInPrimitiveTriangleIndicesEXT) primitiveIndices = true;
            Require((input && ((value == spv::BuiltInWorkgroupId || value == spv::BuiltInLocalInvocationId || value == spv::BuiltInGlobalInvocationId || value == spv::BuiltInNumWorkgroups) && signature == "u32x3")) || (input && value == spv::BuiltInLocalInvocationIndex && signature == "u32") || (!input && value == spv::BuiltInPrimitiveTriangleIndicesEXT && signature == "u32x3") || (!input && value == spv::BuiltInCullPrimitiveEXT && signature == "bool"), "unsupported mesh built-in");
            return;
        }
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

Module Inspect(const CompiledShader& compiled, const State& state) {
    using Stage = ShaderRecompiler::ShaderStage;
    Require(compiled.program != nullptr, "missing compiled shader");
    const auto& shader = *compiled.program;
    const auto stage = compiled.stage;
    const bool vertex = stage == Stage::Vertex || stage == Stage::Local;
    const bool fragment = stage == Stage::Fragment;
    const bool mesh = stage == Stage::Mesh;
    const bool control = stage == Stage::TessellationControl;
    const bool evaluation = stage == Stage::TessellationEvaluation;
    const auto model = mesh ? spv::ExecutionModelMeshEXT : control ? spv::ExecutionModelTessellationControl : evaluation ? spv::ExecutionModelTessellationEvaluation : vertex ? spv::ExecutionModelVertex : spv::ExecutionModelFragment;
    const auto& words = shader.spirv;
    Require(words.size() >= 5 && words[0] == spv::MagicNumber && words[1] >= 0x10000u && words[1] <= 0x10400u && words[3] != 0 && words[4] == 0, "invalid or unsupported SPIR-V header");
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
                Require(count == 2 && (instruction[1] == spv::CapabilityShader || ((control || evaluation) && instruction[1] == spv::CapabilityTessellation) || (mesh && instruction[1] == spv::CapabilityMeshShadingEXT)), "SPIR-V requires an unsupported device capability");
                break;
            case spv::OpExtension: {
                const auto bytes = std::as_bytes(instruction.subspan(1));
                const auto* text = reinterpret_cast<const char*>(bytes.data());
                const auto end = std::find(text, text + bytes.size(), '\0');
                Require(mesh && end != text + bytes.size() && std::string_view(text, static_cast<std::size_t>(end - text)) == "SPV_EXT_mesh_shader", "unsupported SPIR-V extension");
                break;
            }
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
                Require(count >= 5 && instruction[1] == model && instruction[3] == 0x6e69616du && instruction[4] == 0, "expected a main entry point for the assigned graphics stage");
                ++entries;
                for (std::size_t i = 5; i < count; ++i) Require(module.interface.insert(instruction[i]).second, "duplicate SPIR-V interface ID");
                break;
            case spv::OpExecutionMode:
                Require(count >= 3 && module.modes.emplace(instruction[2], std::vector<std::uint32_t>(instruction.begin() + 3, instruction.end())).second, "duplicate or malformed execution mode");
                if (instruction[2] == spv::ExecutionModeOriginUpperLeft) upperLeft = true;
                break;
            case spv::OpExecutionModeId:
                throw std::runtime_error("AGC graphics: execution mode IDs require unsupported specialization");
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
                if (kind == spv::DecorationPatch) decoration.patch = true;
                if (kind == spv::DecorationPerPrimitiveEXT) decoration.perPrimitive = true;
                if (kind == spv::DecorationNonWritable) decoration.nonWritable = true;
                if (kind == spv::DecorationBufferBlock) decoration.bufferBlock = true;
                break;
            }
            case spv::OpMemberDecorate:
                Require(count >= 4, "malformed SPIR-V member decoration");
                if (instruction[3] == spv::DecorationNonWritable) module.readOnlyMembers.emplace(instruction[1], instruction[2]);
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
    Require(!fragment || upperLeft, "fragment coordinates must use an upper-left origin");
    const auto mode = [&](std::uint32_t name, std::vector<std::uint32_t> operands) {
        const auto it = module.modes.find(name);
        Require(it != module.modes.end() && it->second == operands, "missing or incompatible shader execution mode");
    };
    if (mesh) {
        Require(state.stages.mesh.has_value(), "mesh configuration is missing");
        const auto& config = *state.stages.mesh;
        mode(spv::ExecutionModeLocalSize, {config.threadsPerGroup, 1, 1});
        mode(spv::ExecutionModeOutputVertices, {config.maxVertices});
        mode(spv::ExecutionModeOutputPrimitivesEXT, {config.maxPrimitives});
        mode(spv::ExecutionModeOutputTrianglesEXT, {});
        Require(module.modes.size() == 4, "unsupported mesh execution mode");
    } else if (control) {
        Require(state.stages.tessellation.has_value(), "tessellation configuration is missing");
        mode(spv::ExecutionModeOutputVertices, {state.stages.tessellation->outputControlPoints});
        Require(module.modes.size() == 1, "unsupported tessellation-control execution mode");
    } else if (evaluation) {
        mode(spv::ExecutionModeTriangles, {});
        mode(spv::ExecutionModeSpacingFractionalOdd, {});
        mode(spv::ExecutionModeVertexOrderCw, {});
        Require(module.modes.size() == 3, "unsupported tessellation-evaluation execution mode");
    } else if (fragment) {
        for (const auto& [name, operands] : module.modes) Require(operands.empty() && (name == spv::ExecutionModeOriginUpperLeft || name == spv::ExecutionModeEarlyFragmentTests), "unsupported fragment execution mode");
    } else Require(module.modes.empty(), "unsupported vertex execution mode");
    std::set<std::pair<std::uint32_t, std::uint32_t>> descriptors;
    bool push = false;
    for (const auto& [id, variable] : module.variables) {
        if (variable.storage == spv::StorageClassFunction || variable.storage == spv::StorageClassPrivate) continue;
        const auto& pointer = module.Type(variable.pointer);
        Require(pointer.size() == 4 && (pointer[0] & 0xffffu) == spv::OpTypePointer && pointer[2] == variable.storage, "invalid SPIR-V variable pointer");
        auto typeId = pointer[3];
        const auto& decoration = module.decorations[id];
        const bool input = variable.storage == spv::StorageClassInput;
        const bool output = variable.storage == spv::StorageClassOutput;
        const bool tessellationLevels = decoration.builtin && (*decoration.builtin == spv::BuiltInTessLevelOuter || *decoration.builtin == spv::BuiltInTessLevelInner);
        const bool vertexArray = !tessellationLevels && ((control && (input || output)) || (evaluation && input) || (mesh && output)) && !decoration.patch;
        const auto& outer = module.Type(typeId);
        if (vertexArray && (outer[0] & 0xffffu) == spv::OpTypeArray) {
            Require(outer.size() == 4, "malformed per-vertex interface array");
            const auto length = module.constants.find(outer[3]);
            Require(length != module.constants.end() && length->second != 0, "invalid per-vertex interface array length");
            if (mesh) {
                const bool primitive = decoration.perPrimitive || (decoration.builtin && (*decoration.builtin == spv::BuiltInPrimitiveTriangleIndicesEXT || *decoration.builtin == spv::BuiltInCullPrimitiveEXT));
                Require(length->second == (primitive ? state.stages.mesh->maxPrimitives : state.stages.mesh->maxVertices), "mesh interface array disagrees with output limits");
            } else {
                const auto expected = control && input ? state.stages.tessellation->inputControlPoints : state.stages.tessellation->outputControlPoints;
                Require(length->second == expected, "tessellation interface array disagrees with control-point count");
            }
            typeId = outer[2];
        }
        const auto& type = module.Type(typeId);
        if (variable.storage == spv::StorageClassInput || variable.storage == spv::StorageClassOutput) {
            Require(module.interface.contains(id), "SPIR-V input or output is absent from the entry point interface");
            if (decoration.location) {
                Require(!vertexArray || (outer[0] & 0xffffu) == spv::OpTypeArray, "per-vertex interface lacks a control-point or mesh-output dimension");
                Require(!decoration.perPrimitive, "per-primitive user outputs are unsupported");
                Require(!decoration.builtin && !(vertex && variable.storage == spv::StorageClassInput), "vertex attributes require unsupported vertex input bindings");
                auto& locations = variable.storage == spv::StorageClassInput ? module.inputs : module.outputs;
                module.AddLocations(locations, *decoration.location, typeId, decoration.patch);
            } else if (decoration.builtin) {
                module.Builtin(*decoration.builtin, typeId, variable.storage, stage);
            } else {
                Require((type[0] & 0xffffu) == spv::OpTypeStruct, "shader interface lacks a location or built-in");
                for (std::size_t i = 2; i < type.size(); ++i) {
                    const auto builtin = module.builtins.find({typeId, static_cast<std::uint32_t>(i - 2)});
                    Require(builtin != module.builtins.end(), "interface blocks with non-built-in members are unsupported");
                    module.Builtin(builtin->second, type[i], variable.storage, stage);
                }
            }
        } else if (variable.storage == spv::StorageClassPushConstant) {
            Require(!push && !shader.pushConstants.empty() && (type[0] & 0xffffu) == spv::OpTypeStruct, "invalid push constant interface");
            push = true;
            const auto first = compiled.pushConstantOffset;
            for (std::size_t i = 2; i < type.size(); ++i) {
                const auto offset = module.offsets.find({typeId, static_cast<std::uint32_t>(i - 2)});
                Require(offset != module.offsets.end(), "push constant member has no offset");
                Require(offset->second >= first && static_cast<std::uint64_t>(offset->second) + module.Size(type[i]) <= first + shader.pushConstants.size(), "push constant member exceeds its stage range");
            }
        } else if (variable.storage == spv::StorageClassWorkgroup) {
            Require(mesh, "workgroup memory outside a mesh shader");
        } else {
            Require((variable.storage == spv::StorageClassUniform || variable.storage == spv::StorageClassStorageBuffer) && decoration.set && decoration.binding, "unsupported or unbound shader resource");
            Require((type[0] & 0xffffu) == spv::OpTypeStruct, "descriptor arrays and non-buffer shader resources are unsupported");
            const auto key = std::make_pair(*decoration.set, *decoration.binding);
            Require(descriptors.insert(key).second, "duplicate SPIR-V resource binding");
            const auto binding = std::find_if(shader.bindings.begin(), shader.bindings.end(), [&](const auto& item) { return item.descriptorSet == key.first && item.binding == key.second; });
            Require(binding != shader.bindings.end(), "SPIR-V resource is absent from recompiler binding metadata");
            const auto kind = variable.storage == spv::StorageClassStorageBuffer || module.decorations[typeId].bufferBlock ? ShaderRecompiler::DescriptorKind::StorageBuffer : ShaderRecompiler::DescriptorKind::UniformBuffer;
            if (kind == ShaderRecompiler::DescriptorKind::StorageBuffer && binding->readOnly) {
                bool membersReadOnly = type.size() > 2;
                for (std::size_t member = 2; member < type.size(); ++member) membersReadOnly = membersReadOnly && module.readOnlyMembers.contains({typeId, static_cast<std::uint32_t>(member - 2)});
                Require(decoration.nonWritable || membersReadOnly, "read-only buffer metadata lacks NonWritable decoration");
            }
            Require(binding->kind == kind, "SPIR-V descriptor type disagrees with recompiler binding metadata");
        }
    }
    Require(descriptors.size() == shader.bindings.size(), "recompiler binding metadata contains undeclared resources");
    Require(push == !shader.pushConstants.empty(), "recompiler push constant metadata disagrees with SPIR-V");
    for (const auto id : module.interface) Require(module.variables.contains(id), "entry point interface contains an unknown variable");
    if (mesh) Require(module.primitiveIndices, "mesh shader does not export primitive indices");
    if (stage == Stage::Vertex || mesh || evaluation) Require(module.position, "vertex shader does not export position");
    return module;
}

}

void ValidateShaders(std::span<const CompiledShader> shaders, const State& state) {
    using Stage = ShaderRecompiler::ShaderStage;
    const bool tessellation = state.stages.path == ShaderPath::Tessellation;
    Require(shaders.size() == (tessellation ? 4u : 2u), "incorrect graphics stage count");
    const std::array<Stage, 4> tessStages{Stage::Local, Stage::TessellationControl, Stage::TessellationEvaluation, Stage::Fragment};
    Module previous;
    for (std::size_t i = 0; i < shaders.size(); ++i) {
        const auto expected = tessellation ? tessStages[i] : i == 1 ? Stage::Fragment : state.stages.path == ShaderPath::Geometry ? Stage::Mesh : Stage::Vertex;
        Require(shaders[i].program != nullptr && shaders[i].program->pushConstants.size() <= PushConstantStride(shaders.size()), "graphics push constants exceed the assigned range");
        Require(shaders[i].stage == expected && shaders[i].pushConstantOffset == i * PushConstantStride(shaders.size()), "graphics stage order or push constant layout disagrees");
        for (const auto& binding : shaders[i].program->bindings) Require(binding.descriptorSet == i, "graphics resource uses a different stage descriptor set");
        const auto current = Inspect(shaders[i], state);
        if (i != 0) {
            for (const auto& [location, signature] : current.inputs) {
                const auto output = previous.outputs.find(location);
                Require(output != previous.outputs.end() && output->second == signature, "graphics interfaces disagree at location " + std::to_string(location));
            }
        }
        previous = current;
    }
    Require(previous.outputs.size() == 1 && previous.outputs.contains(0) && previous.outputs.at(0) == "vertex:f32x4", "fragment shader must export one float4 color at location zero");
}

void ValidateShaderPair(const ShaderRecompiler::RecompileResult& vertex, const ShaderRecompiler::RecompileResult& fragment) {
    const std::array<CompiledShader, 2> shaders{{{ShaderRecompiler::ShaderStage::Vertex, &vertex, 0}, {ShaderRecompiler::ShaderStage::Fragment, &fragment, StagePushConstantBytes}}};
    State state{};
    state.stages.path = ShaderPath::Vertex;
    ValidateShaders(shaders, state);
}

}
