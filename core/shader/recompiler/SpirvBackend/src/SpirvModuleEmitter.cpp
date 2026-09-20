#include "SpirvBackend/SpirvModuleEmitter.hpp"
#include "SpirvBackend/SpirvEmitterHelpers.hpp"
#include "SpirvBackend/SpirvEmitterInstructions.hpp"
#include <spirv/unified1/GLSL.std.450.h>
#include <spirv/unified1/spirv.hpp>
#include <algorithm>
#include <array>
#include <limits>
#include <stdexcept>

namespace ShaderRecompiler {

namespace {

bool UserDataDwordIndex(const SpirvEmitterState& state, ScalarReg reg, std::uint32_t& dwordIndex) {
    const auto registerIndex = RegIndex(reg);
    const auto& registers = state.program.Metadata().bindings.userDataRegisters;
    const auto found = std::lower_bound(registers.begin(), registers.end(), registerIndex);
    if (found == registers.end() || *found != registerIndex) {
        return false;
    }
    dwordIndex = static_cast<std::uint32_t>(found - registers.begin());
    return true;
}

const ShaderWorkgroupInputInfo* ShaderWorkgroupInputFor(const SpirvEmitterState& state) {
    switch (state.program.Resources().stage) {
    case IrShaderStage::Compute:
        return state.inputInfo.compute;
    case IrShaderStage::Mesh:
        return state.inputInfo.vertex != nullptr ? &state.inputInfo.vertex->mesh : nullptr;
    default:
        return nullptr;
    }
}

std::uint32_t EmitBuiltinU32(SpirvEmitterState& state, StageInputKind kind, std::uint32_t component) {
    if (kind == StageInputKind::LocalInvocationIndex) {
        return EmitLocalInvocationIndex(state);
    }
    if (state.laneCount == 2 && (kind == StageInputKind::LocalInvocationId || kind == StageInputKind::GlobalInvocationId)) {
        const auto* workgroup = ShaderWorkgroupInputFor(state);
        if (workgroup == nullptr) {
            throw std::runtime_error("workgroup input info is missing for a local/global invocation builtin");
        }
        std::uint32_t divisor = 1u;
        for (std::uint32_t axis = 0; axis < component; axis++) {
            divisor *= std::max(workgroup->threadsNum[axis], 1u);
        }
        const auto size = std::max(workgroup->threadsNum[component], 1u);
        const auto divided = EmitBinaryU32(state, spv::OpUDiv, EmitLocalInvocationIndex(state), ConstantU32(state, divisor));
        const auto local = EmitBinaryU32(state, spv::OpUMod, divided, ConstantU32(state, size));
        if (kind == StageInputKind::LocalInvocationId) {
            return local;
        }
        const auto group = EmitInputComponentU32(state, StageInputKind::WorkgroupId, component);
        return EmitBinaryU32(state, spv::OpIAdd, local, EmitBinaryU32(state, spv::OpIMul, group, ConstantU32(state, size)));
    }
    const auto variable = InputVariableForKind(state, kind);
    if (variable == 0) {
        return ConstantU32(state, 0u);
    }
    if (kind == StageInputKind::FrontFacing) {
        const auto value = state.module.AllocateId();
        const auto bits = state.module.AllocateId();
        state.module.AddFunction(spv::OpLoad, TypeBool(state), value, variable);
        state.module.AddFunction(spv::OpSelect, TypeU32(state), bits, value, ConstantU32(state, 0x3f800000u), ConstantU32(state, 0xbf800000u));
        return bits;
    }
    if (kind == StageInputKind::VertexIndex || kind == StageInputKind::InstanceIndex || kind == StageInputKind::InvocationId || kind == StageInputKind::PrimitiveId || kind == StageInputKind::Layer || kind == StageInputKind::SampleId) {
        const auto value = state.module.AllocateId();
        const auto bits = state.module.AllocateId();
        state.module.AddFunction(spv::OpLoad, TypeI32(state), value, variable);
        state.module.AddFunction(spv::OpBitcast, TypeU32(state), bits, value);
        return bits;
    }
    if (kind == StageInputKind::FragCoord || kind == StageInputKind::TessCoord) {
        const auto pointer = state.module.AllocateId();
        const auto value = state.module.AllocateId();
        const auto bits = state.module.AllocateId();
        state.module.AddFunction(spv::OpAccessChain, TypePointer(state, spv::StorageClassInput, TypeF32(state)), pointer, variable, ConstantU32(state, component));
        state.module.AddFunction(spv::OpLoad, TypeF32(state), value, pointer);
        state.module.AddFunction(spv::OpBitcast, TypeU32(state), bits, value);
        return bits;
    }
    if (kind == StageInputKind::BaryCoordSmooth || kind == StageInputKind::BaryCoordNoPerspective) {
        const auto pointer = state.module.AllocateId();
        const auto value = state.module.AllocateId();
        const auto bits = state.module.AllocateId();
        state.module.AddFunction(spv::OpAccessChain, TypePointer(state, spv::StorageClassInput, TypeF32(state)), pointer, variable, ConstantU32(state, component + 1u));
        state.module.AddFunction(spv::OpLoad, TypeF32(state), value, pointer);
        state.module.AddFunction(spv::OpBitcast, TypeU32(state), bits, value);
        return bits;
    }
    return EmitInputComponentU32(state, kind, component);
}

std::uint32_t EmitAttributeValue(SpirvEmitterState& state, std::uint32_t attr, std::uint32_t chan) {
    const auto* input = SpirvInputBindingForParameter(state, attr);
    if (input == nullptr || input->variableId == 0) {
        return ConstantU32(state, 0u);
    }
    const auto stage = state.program.Resources().stage;
    if (stage == IrShaderStage::Vertex || stage == IrShaderStage::Local) {
        return EmitVertexParameterComponentU32(state, *input, chan & 3u);
    }
    const auto loadPerVertex = [&](std::uint32_t vertex) {
        const auto pointer = state.module.AllocateId();
        const auto value = state.module.AllocateId();
        state.module.AddFunction(spv::OpAccessChain, TypePointer(state, spv::StorageClassInput, TypeF32(state)), pointer, input->variableId, ConstantU32(state, vertex), ConstantU32(state, chan & 3u));
        state.module.AddFunction(spv::OpLoad, TypeF32(state), value, pointer);
        return value;
    };
    if (input->perVertex) {
        if (state.inputInfo.pixel == nullptr) {
            throw std::runtime_error("pixel input info is missing for a per-vertex attribute");
        }
        const auto barycentricKind = state.inputInfo.pixel->psNoPerspective ? StageInputKind::BaryCoordNoPerspective : StageInputKind::BaryCoordSmooth;
        const auto barycentric = InputVariableForKind(state, barycentricKind);
        std::uint32_t sum = 0;
        for (std::uint32_t vertex = 0; vertex < 3u; vertex++) {
            const auto pointer = state.module.AllocateId();
            const auto weight = state.module.AllocateId();
            const auto product = state.module.AllocateId();
            state.module.AddFunction(spv::OpAccessChain, TypePointer(state, spv::StorageClassInput, TypeF32(state)), pointer, barycentric, ConstantU32(state, vertex));
            state.module.AddFunction(spv::OpLoad, TypeF32(state), weight, pointer);
            state.module.AddFunction(spv::OpFMul, TypeF32(state), product, loadPerVertex(vertex), weight);
            if (vertex == 0u) {
                sum = product;
            } else {
                const auto next = state.module.AllocateId();
                state.module.AddFunction(spv::OpFAdd, TypeF32(state), next, sum, product);
                sum = next;
            }
        }
        const auto bits = state.module.AllocateId();
        state.module.AddFunction(spv::OpBitcast, TypeU32(state), bits, sum);
        return bits;
    }
    const auto vector = state.module.AllocateId();
    const auto component = state.module.AllocateId();
    const auto bits = state.module.AllocateId();
    state.module.AddFunction(spv::OpLoad, TypeF32Vector(state, 4u), vector, input->variableId);
    state.module.AddFunction(spv::OpCompositeExtract, TypeF32(state), component, vector, chan & 3u);
    state.module.AddFunction(spv::OpBitcast, TypeU32(state), bits, component);
    return bits;
}

std::uint32_t EmitInterpolationParameterValue(SpirvEmitterState& state, std::uint32_t attr, std::uint32_t chan, std::uint32_t mode) {
    const auto* input = SpirvInputBindingForParameter(state, attr);
    if (input == nullptr) {
        throw std::runtime_error("interpolation parameter refers to an undefined attribute");
    }
    if (!input->perVertex) {
        return EmitAttributeValue(state, attr, chan);
    }
    const auto loadVertex = [&](std::uint32_t vertex) {
        const auto pointer = state.module.AllocateId();
        const auto value = state.module.AllocateId();
        state.module.AddFunction(spv::OpAccessChain, TypePointer(state, spv::StorageClassInput, TypeF32(state)), pointer, input->variableId, ConstantU32(state, vertex), ConstantU32(state, chan & 3u));
        state.module.AddFunction(spv::OpLoad, TypeF32(state), value, pointer);
        return value;
    };
    const auto selectedVertex = (mode + 1u) % 3u;
    std::uint32_t value = loadVertex(selectedVertex);
    if (!PixelParameterIsCustom(state, attr) && mode < 2u) {
        const auto delta = state.module.AllocateId();
        state.module.AddFunction(spv::OpFSub, TypeF32(state), delta, value, loadVertex(0u));
        value = delta;
    }
    const auto bits = state.module.AllocateId();
    state.module.AddFunction(spv::OpBitcast, TypeU32(state), bits, value);
    return bits;
}

std::uint32_t MrtOutputMode(const SpirvEmitterState& state, const ExportInfo& exp) {
    if (state.program.Resources().stage != IrShaderStage::Pixel || exp.kind != ExportTargetKind::Mrt) {
        return 0u;
    }
    if (state.inputInfo.pixel == nullptr || exp.index >= std::size(state.inputInfo.pixel->targetOutputMode)) {
        return 0u;
    }
    return state.inputInfo.pixel->targetOutputMode[exp.index];
}

std::uint32_t ExportRawComponent(SpirvValueEmitContext& ctx, std::uint32_t vector, std::uint32_t component) {
    const auto value = ctx.state.module.AllocateId();
    ctx.state.module.AddFunction(spv::OpCompositeExtract, TypeU32(ctx.state), value, vector, component);
    return value;
}

std::uint32_t ExportVector(SpirvValueEmitContext& ctx, std::uint32_t data, const ExportInfo& exp, bool uintOutput) {
    auto& state = ctx.state;
    if (exp.compr && !uintOutput) {
        const auto unpack = MrtOutputMode(state, exp) == 5u ? GLSLstd450UnpackUnorm2x16 : GLSLstd450UnpackHalf2x16;
        std::array<std::uint32_t, 4> f32 {ConstantF32(state, 0u), ConstantF32(state, 0u), ConstantF32(state, 0u), ConstantF32(state, 0x3f800000u)};
        for (std::uint32_t pair = 0; pair < 2u; pair++) {
            if ((exp.en & (3u << (pair * 2u))) == 0u) {
                continue;
            }
            const auto packed = state.module.AllocateId();
            const auto unpacked = state.module.AllocateId();
            state.module.AddFunction(spv::OpCompositeExtract, TypeU32(state), packed, data, pair);
            state.module.AddFunction(spv::OpExtInst, TypeF32Vector(state, 2u), unpacked, GlslStd450(state), unpack, packed);
            for (std::uint32_t lane = 0; lane < 2u; lane++) {
                const auto component = pair * 2u + lane;
                if (((exp.en >> component) & 1u) != 0u) {
                    f32.at(component) = state.module.AllocateId();
                    state.module.AddFunction(spv::OpCompositeExtract, TypeF32(state), f32.at(component), unpacked, lane);
                }
            }
        }
        const auto vector = state.module.AllocateId();
        state.module.AddFunction(spv::OpCompositeConstruct, TypeF32Vector(state, 4u), vector, f32[0], f32[1], f32[2], f32[3]);
        return vector;
    }
    std::array<std::uint32_t, 4> raw {
        ConstantU32(state, 0u),
        ConstantU32(state, 0u),
        ConstantU32(state, 0u),
        ConstantU32(state, uintOutput ? 1u : 0x3f800000u),
    };
    if (exp.compr) {
        for (std::uint32_t pair = 0; pair < 2u; pair++) {
            if ((exp.en & (3u << (pair * 2u))) == 0u) {
                continue;
            }
            const auto packed = ExportRawComponent(ctx, data, pair);
            for (std::uint32_t lane = 0; lane < 2u; lane++) {
                const auto component = pair * 2u + lane;
                if (((exp.en >> component) & 1u) == 0u) {
                    continue;
                }
                raw.at(component) = state.module.AllocateId();
                state.module.AddFunction(spv::OpBitFieldUExtract, TypeU32(state), raw.at(component), packed, ConstantU32(state, lane * 16u), ConstantU32(state, 16u));
            }
        }
    } else {
        for (std::uint32_t component = 0; component < 4u; component++) {
            if (((exp.en >> component) & 1u) != 0u) {
                raw.at(component) = ExportRawComponent(ctx, data, component);
            }
        }
    }
    if (uintOutput) {
        const auto vector = state.module.AllocateId();
        state.module.AddFunction(spv::OpCompositeConstruct, TypeU32Vector(state, 4u), vector, raw[0], raw[1], raw[2], raw[3]);
        return vector;
    }
    std::array<std::uint32_t, 4> f32 {};
    for (std::uint32_t component = 0; component < 4u; component++) {
        f32.at(component) = state.module.AllocateId();
        state.module.AddFunction(spv::OpBitcast, TypeF32(state), f32.at(component), raw.at(component));
    }
    const auto vector = state.module.AllocateId();
    state.module.AddFunction(spv::OpCompositeConstruct, TypeF32Vector(state, 4u), vector, f32[0], f32[1], f32[2], f32[3]);
    return vector;
}

void EmitAuxPositionExport(SpirvValueEmitContext& ctx, std::uint32_t data, const ExportInfo& exp) {
    auto& state = ctx.state;
    if (state.inputInfo.vertex == nullptr) {
        throw std::runtime_error("vertex input info is missing for an auxiliary position export");
    }
    for (std::uint32_t component = 0; component < 4u; component++) {
        if ((exp.en & (1u << component)) == 0u) {
            continue;
        }
        const auto output = DecodePositionExportComponent(state.inputInfo.vertex->paClVsOutCntl, exp.index, component);
        if (output.layer || output.viewport) {
            const auto raw = ExportRawComponent(ctx, data, component);
            if (output.layer) {
                const auto layer = state.module.AllocateId();
                state.module.AddFunction(spv::OpBitwiseAnd, TypeU32(state), layer, raw, ConstantU32(state, 0x7ffu));
                const auto pointer = state.program.Resources().stage == IrShaderStage::Mesh ? MeshOutputPointer(state, StageOutputKind::Layer) : state.layerVariable;
                state.module.AddFunction(spv::OpStore, pointer, layer);
            }
            if (output.viewport) {
                const auto viewport = state.module.AllocateId();
                state.module.AddFunction(spv::OpBitFieldUExtract, TypeU32(state), viewport, raw, ConstantU32(state, 16u), ConstantU32(state, 4u));
                state.module.AddFunction(spv::OpStore, state.viewportIndexVariable, viewport);
            }
            continue;
        }
        if (!output.pointSize && output.clipDistance == std::numeric_limits<std::uint32_t>::max() && output.cullDistance == std::numeric_limits<std::uint32_t>::max()) {
            continue;
        }
        const auto raw = ExportRawComponent(ctx, data, component);
        const auto f32 = state.module.AllocateId();
        state.module.AddFunction(spv::OpBitcast, TypeF32(state), f32, raw);
        if (output.pointSize) {
            state.module.AddFunction(spv::OpStore, state.pointSizeVariable, f32);
            continue;
        }
        const auto storeDistance = [&](std::uint32_t variable, std::uint32_t index) {
            if (index == std::numeric_limits<std::uint32_t>::max()) {
                return;
            }
            const auto pointer = state.module.AllocateId();
            state.module.AddFunction(spv::OpAccessChain, TypePointer(state, spv::StorageClassOutput, TypeF32(state)), pointer, variable, ConstantU32(state, index));
            state.module.AddFunction(spv::OpStore, pointer, f32);
        };
        storeDistance(state.clipDistanceVariable, output.clipDistance);
        storeDistance(state.cullDistanceVariable, output.cullDistance);
    }
}

std::uint32_t ConvertClipCoordinate(SpirvEmitterState& state, std::uint32_t coordinate, float scale, float offset, float halfExtent) {
    const auto window = state.module.AllocateId();
    const auto biased = state.module.AllocateId();
    const auto divided = state.module.AllocateId();
    const auto ndc = state.module.AllocateId();
    state.module.AddFunction(spv::OpFMul, TypeF32(state), window, coordinate, ConstantF32Value(state, scale));
    state.module.AddFunction(spv::OpFAdd, TypeF32(state), biased, window, ConstantF32Value(state, offset));
    state.module.AddFunction(spv::OpFDiv, TypeF32(state), divided, biased, ConstantF32Value(state, halfExtent));
    state.module.AddFunction(spv::OpFSub, TypeF32(state), ndc, divided, ConstantF32Value(state, 1.0f));
    return ndc;
}

std::uint32_t ConvertPositionToClipSpace(SpirvEmitterState& state, std::uint32_t position) {
    if (state.inputInfo.vertex == nullptr) {
        throw std::runtime_error("vertex input info is missing for clip-space conversion");
    }
    const auto& transform = state.inputInfo.vertex->clipSpace;
    std::array<std::uint32_t, 4> components {};
    for (std::uint32_t i = 0; i < 4u; i++) {
        components.at(i) = state.module.AllocateId();
        state.module.AddFunction(spv::OpCompositeExtract, TypeF32(state), components.at(i), position, i);
    }
    components[0] = ConvertClipCoordinate(state, components[0], transform.scale[0], transform.offset[0], transform.halfExtent[0]);
    components[1] = ConvertClipCoordinate(state, components[1], transform.scale[1], transform.offset[1], transform.halfExtent[1]);
    const auto converted = state.module.AllocateId();
    state.module.AddFunction(spv::OpCompositeConstruct, TypeF32Vector(state, 4u), converted, components[0], components[1], components[2], components[3]);
    return converted;
}

}

void DefineTessellationInterfaces(SpirvEmitterState& state) {
    throw std::runtime_error("DefineTessellationInterfaces not implemented");
}

void DefineTessellationExecutionModes(SpirvEmitterState& state) {
    throw std::runtime_error("DefineTessellationExecutionModes not implemented");
}

void DefineMeshOutputs(SpirvEmitterState& state) {
    throw std::runtime_error("DefineMeshOutputs not implemented");
}

void EmitMeshEntryPoint(SpirvEmitterState& state) {
    throw std::runtime_error("EmitMeshEntryPoint not implemented");
}

void EmitMeshAllocate(SpirvValueEmitContext& ctx, const IrValue& inst) {
    throw std::runtime_error("EmitMeshAllocate not implemented");
}

std::uint32_t MeshOutputPointer(SpirvEmitterState& state, StageOutputKind kind, std::uint32_t index) {
    throw std::runtime_error("MeshOutputPointer not implemented");
}

std::uint32_t MeshPrimitivePointer(SpirvEmitterState& state) {
    throw std::runtime_error("MeshPrimitivePointer not implemented");
}

std::uint32_t EmitAndConstant(SpirvEmitterState& state, std::uint32_t value, std::uint32_t mask) {
    const auto result = state.module.AllocateId();
    state.module.AddFunction(spv::OpBitwiseAnd, TypeU32(state), result, value, ConstantU32(state, mask));
    return result;
}

std::uint32_t EmitShiftRightConstant(SpirvEmitterState& state, std::uint32_t value, std::uint32_t shift) {
    const auto result = state.module.AllocateId();
    state.module.AddFunction(spv::OpShiftRightLogical, TypeU32(state), result, value, ConstantU32(state, shift));
    return result;
}

std::uint32_t EmitCompareU32Constant(SpirvEmitterState& state, std::uint32_t opcode, std::uint32_t value, std::uint32_t constant) {
    const auto result = state.module.AllocateId();
    state.module.AddFunction(opcode, TypeBool(state), result, value, ConstantU32(state, constant));
    return result;
}

std::uint32_t EmitSubConstantMinusU32(SpirvEmitterState& state, std::uint32_t constant, std::uint32_t value) {
    const auto result = state.module.AllocateId();
    state.module.AddFunction(spv::OpISub, TypeU32(state), result, ConstantU32(state, constant), value);
    return result;
}

std::uint32_t EmitF32ToF16RtzBits(SpirvEmitterState& state, std::uint32_t f32) {
    const auto bits = state.module.AllocateId();
    state.module.AddFunction(spv::OpBitcast, TypeU32(state), bits, f32);

    const auto sign = EmitAndConstant(state, EmitShiftRightConstant(state, bits, 16u), 0x8000u);
    const auto exponent = EmitAndConstant(state, EmitShiftRightConstant(state, bits, 23u), 0xffu);
    const auto mantissa = EmitAndConstant(state, bits, 0x007fffffu);

    const auto halfExponent = state.module.AllocateId();
    const auto normalExponent = state.module.AllocateId();
    const auto normalMantissa = EmitShiftRightConstant(state, mantissa, 13u);
    const auto normalPayload = state.module.AllocateId();
    const auto normal = state.module.AllocateId();
    state.module.AddFunction(spv::OpISub, TypeU32(state), halfExponent, exponent, ConstantU32(state, 112u));
    state.module.AddFunction(spv::OpShiftLeftLogical, TypeU32(state), normalExponent, halfExponent, ConstantU32(state, 10u));
    state.module.AddFunction(spv::OpBitwiseOr, TypeU32(state), normalPayload, normalExponent, normalMantissa);
    state.module.AddFunction(spv::OpBitwiseOr, TypeU32(state), normal, sign, normalPayload);

    const auto mantissaWithHidden = Binary(state, spv::OpBitwiseOr, TypeU32(state), mantissa, ConstantU32(state, 0x00800000u));
    const auto rawSubShift = EmitSubConstantMinusU32(state, 126u, exponent);
    const auto exponentLt103 = EmitCompareU32Constant(state, spv::OpULessThan, exponent, 103u);
    const auto exponentGt112 = EmitCompareU32Constant(state, spv::OpUGreaterThan, exponent, 112u);
    const auto subShiftLow = Select(state, TypeU32(state), exponentLt103, ConstantU32(state, 31u), rawSubShift);
    const auto subShift = Select(state, TypeU32(state), exponentGt112, ConstantU32(state, 14u), subShiftLow);
    const auto subMantissa = state.module.AllocateId();
    const auto subnormal = state.module.AllocateId();
    state.module.AddFunction(spv::OpShiftRightLogical, TypeU32(state), subMantissa, mantissaWithHidden, subShift);
    state.module.AddFunction(spv::OpBitwiseOr, TypeU32(state), subnormal, sign, subMantissa);

    const auto nanPayload = Binary(state, spv::OpBitwiseOr, TypeU32(state), EmitShiftRightConstant(state, mantissa, 13u), ConstantU32(state, 0x0200u));
    const auto nan = Binary(state, spv::OpBitwiseOr, TypeU32(state), sign, Binary(state, spv::OpBitwiseOr, TypeU32(state), ConstantU32(state, 0x7c00u), nanPayload));
    const auto inf = Binary(state, spv::OpBitwiseOr, TypeU32(state), sign, ConstantU32(state, 0x7c00u));
    const auto maxFinite = Binary(state, spv::OpBitwiseOr, TypeU32(state), sign, ConstantU32(state, 0x7bffu));
    const auto mantissaZero = EmitCompareU32Constant(state, spv::OpIEqual, mantissa, 0u);
    const auto special = Select(state, TypeU32(state), mantissaZero, inf, nan);

    const auto exponentLe112 = EmitCompareU32Constant(state, spv::OpULessThanEqual, exponent, 112u);
    const auto exponentGe143 = EmitCompareU32Constant(state, spv::OpUGreaterThanEqual, exponent, 143u);
    const auto exponentEq255 = EmitCompareU32Constant(state, spv::OpIEqual, exponent, 255u);
    const auto finite0 = Select(state, TypeU32(state), exponentLe112, subnormal, normal);
    const auto finite1 = Select(state, TypeU32(state), exponentLt103, sign, finite0);
    const auto finite2 = Select(state, TypeU32(state), exponentGe143, maxFinite, finite1);
    return EmitAndConstant(state, Select(state, TypeU32(state), exponentEq255, special, finite2), 0xffffu);
}

std::uint32_t EmitMinMaxU32Value(SpirvEmitterState& state, std::uint32_t lhs, std::uint32_t rhs, bool maxValue) {
    const auto condition = Binary(state, maxValue ? spv::OpUGreaterThan : spv::OpULessThan, TypeBool(state), lhs, rhs);
    return Select(state, TypeU32(state), condition, lhs, rhs);
}

std::uint32_t EmitMinMaxI32Value(SpirvEmitterState& state, std::uint32_t lhs, std::uint32_t rhs, bool maxValue) {
    const auto condition = Binary(state, maxValue ? spv::OpSGreaterThan : spv::OpSLessThan, TypeBool(state), lhs, rhs);
    return Select(state, TypeU32(state), condition, lhs, rhs);
}

F32Class EmitClassifyF32Bits(SpirvEmitterState& state, std::uint32_t bits) {
    F32Class cls;
    cls.bits = bits;
    const auto absBits = EmitAndConstant(state, cls.bits, 0x7fffffffu);
    const auto exponentBits = EmitAndConstant(state, absBits, 0x7f800000u);
    const auto mantissaBits = EmitAndConstant(state, absBits, 0x007fffffu);
    const auto exponentMax = EmitCompareU32Constant(state, spv::OpIEqual, exponentBits, 0x7f800000u);
    const auto mantissaNonzero = EmitCompareU32Constant(state, spv::OpINotEqual, mantissaBits, 0u);
    cls.nan = Binary(state, spv::OpLogicalAnd, TypeBool(state), exponentMax, mantissaNonzero);
    cls.zero = EmitCompareU32Constant(state, spv::OpIEqual, absBits, 0u);
    return cls;
}

F32Class EmitClassifyF32(SpirvEmitterState& state, std::uint32_t value) {
    return EmitClassifyF32Bits(state, Unary(state, spv::OpBitcast, TypeU32(state), value));
}

std::uint32_t EmitClassMaskBitMatch(SpirvEmitterState& state, std::uint32_t mask, std::uint32_t bit, std::uint32_t classMatch) {
    const auto selected = EmitCompareU32Constant(state, spv::OpINotEqual, EmitAndConstant(state, mask, 1u << bit), 0u);
    return Binary(state, spv::OpLogicalAnd, TypeBool(state), selected, classMatch);
}

std::uint32_t EmitClassMaskF32(SpirvEmitterState& state, std::uint32_t value, std::uint32_t mask) {
    const auto bits = Unary(state, spv::OpBitcast, TypeU32(state), value);
    const auto signBits = EmitAndConstant(state, bits, 0x80000000u);
    const auto absBits = EmitAndConstant(state, bits, 0x7fffffffu);
    const auto exponentBits = EmitAndConstant(state, absBits, 0x7f800000u);
    const auto mantissaBits = EmitAndConstant(state, absBits, 0x007fffffu);
    const auto quietBits = EmitAndConstant(state, mantissaBits, 0x00400000u);

    const auto sign = EmitCompareU32Constant(state, spv::OpINotEqual, signBits, 0u);
    const auto positive = Unary(state, spv::OpLogicalNot, TypeBool(state), sign);
    const auto exponentZero = EmitCompareU32Constant(state, spv::OpIEqual, exponentBits, 0u);
    const auto exponentNonzero = Unary(state, spv::OpLogicalNot, TypeBool(state), exponentZero);
    const auto exponentInf = EmitCompareU32Constant(state, spv::OpIEqual, exponentBits, 0x7f800000u);
    const auto finiteExponent = Unary(state, spv::OpLogicalNot, TypeBool(state), exponentInf);
    const auto mantissaZero = EmitCompareU32Constant(state, spv::OpIEqual, mantissaBits, 0u);
    const auto mantissaNonzero = Unary(state, spv::OpLogicalNot, TypeBool(state), mantissaZero);
    const auto quiet = EmitCompareU32Constant(state, spv::OpINotEqual, quietBits, 0u);

    const auto nanCommon = Binary(state, spv::OpLogicalAnd, TypeBool(state), exponentInf, mantissaNonzero);
    const auto snan = Binary(state, spv::OpLogicalAnd, TypeBool(state), nanCommon, Unary(state, spv::OpLogicalNot, TypeBool(state), quiet));
    const auto qnan = Binary(state, spv::OpLogicalAnd, TypeBool(state), nanCommon, quiet);
    const auto inf = Binary(state, spv::OpLogicalAnd, TypeBool(state), exponentInf, mantissaZero);
    const auto normal = Binary(state, spv::OpLogicalAnd, TypeBool(state), exponentNonzero, finiteExponent);
    const auto denorm = Binary(state, spv::OpLogicalAnd, TypeBool(state), exponentZero, mantissaNonzero);
    const auto zero = EmitCompareU32Constant(state, spv::OpIEqual, absBits, 0u);

    std::uint32_t match = EmitClassMaskBitMatch(state, mask, 0u, snan);
    match = Binary(state, spv::OpLogicalOr, TypeBool(state), match, EmitClassMaskBitMatch(state, mask, 1u, qnan));
    match = Binary(state, spv::OpLogicalOr, TypeBool(state), match, EmitClassMaskBitMatch(state, mask, 2u, Binary(state, spv::OpLogicalAnd, TypeBool(state), inf, sign)));
    match = Binary(state, spv::OpLogicalOr, TypeBool(state), match, EmitClassMaskBitMatch(state, mask, 3u, Binary(state, spv::OpLogicalAnd, TypeBool(state), normal, sign)));
    match = Binary(state, spv::OpLogicalOr, TypeBool(state), match, EmitClassMaskBitMatch(state, mask, 4u, Binary(state, spv::OpLogicalAnd, TypeBool(state), denorm, sign)));
    match = Binary(state, spv::OpLogicalOr, TypeBool(state), match, EmitClassMaskBitMatch(state, mask, 5u, Binary(state, spv::OpLogicalAnd, TypeBool(state), zero, sign)));
    match = Binary(state, spv::OpLogicalOr, TypeBool(state), match, EmitClassMaskBitMatch(state, mask, 6u, Binary(state, spv::OpLogicalAnd, TypeBool(state), zero, positive)));
    match = Binary(state, spv::OpLogicalOr, TypeBool(state), match, EmitClassMaskBitMatch(state, mask, 7u, Binary(state, spv::OpLogicalAnd, TypeBool(state), denorm, positive)));
    match = Binary(state, spv::OpLogicalOr, TypeBool(state), match, EmitClassMaskBitMatch(state, mask, 8u, Binary(state, spv::OpLogicalAnd, TypeBool(state), normal, positive)));
    return Binary(state, spv::OpLogicalOr, TypeBool(state), match, EmitClassMaskBitMatch(state, mask, 9u, Binary(state, spv::OpLogicalAnd, TypeBool(state), inf, positive)));
}

std::uint32_t EmitMinMaxF32Value(SpirvEmitterState& state, std::uint32_t lhs, std::uint32_t rhs, bool maxValue) {
    const auto lhsClass = EmitClassifyF32(state, lhs);
    const auto rhsClass = EmitClassifyF32(state, rhs);

    const auto numericCond = Binary(state, maxValue ? spv::OpFOrdGreaterThanEqual : spv::OpFOrdLessThan, TypeBool(state), lhs, rhs);
    const auto orderedBits = Select(state, TypeU32(state), numericCond, lhsClass.bits, rhsClass.bits);

    const auto bothZero = Binary(state, spv::OpLogicalAnd, TypeBool(state), lhsClass.zero, rhsClass.zero);
    const auto zeroBits = Binary(state, maxValue ? spv::OpBitwiseAnd : spv::OpBitwiseOr, TypeU32(state), lhsClass.bits, rhsClass.bits);
    const auto numericBits = Select(state, TypeU32(state), bothZero, zeroBits, orderedBits);

    const auto rhsNanBits = Select(state, TypeU32(state), rhsClass.nan, lhsClass.bits, numericBits);
    const auto resultBits = Select(state, TypeU32(state), lhsClass.nan, rhsClass.bits, rhsNanBits);
    return Unary(state, spv::OpBitcast, TypeF32(state), resultBits);
}

std::uint32_t EmitFlushF32DenormToSignedZero(SpirvEmitterState& state, std::uint32_t value) {
    const auto bits = state.module.AllocateId();
    const auto absBits = state.module.AllocateId();
    const auto signBits = state.module.AllocateId();
    const auto nonZero = state.module.AllocateId();
    const auto subnormal = state.module.AllocateId();
    const auto flush = state.module.AllocateId();
    const auto selected = state.module.AllocateId();
    const auto result = state.module.AllocateId();
    state.module.AddFunction(spv::OpBitcast, TypeU32(state), bits, value);
    state.module.AddFunction(spv::OpBitwiseAnd, TypeU32(state), absBits, bits, ConstantU32(state, 0x7fffffffu));
    state.module.AddFunction(spv::OpBitwiseAnd, TypeU32(state), signBits, bits, ConstantU32(state, 0x80000000u));
    state.module.AddFunction(spv::OpINotEqual, TypeBool(state), nonZero, absBits, ConstantU32(state, 0u));
    state.module.AddFunction(spv::OpULessThan, TypeBool(state), subnormal, absBits, ConstantU32(state, 0x00800000u));
    state.module.AddFunction(spv::OpLogicalAnd, TypeBool(state), flush, nonZero, subnormal);
    state.module.AddFunction(spv::OpSelect, TypeU32(state), selected, flush, signBits, bits);
    state.module.AddFunction(spv::OpBitcast, TypeF32(state), result, selected);
    return result;
}

std::uint32_t EmitTrigCycleF32(SpirvEmitterState& state, std::uint32_t src, bool preserveSignedZero) {
    const auto fract = state.module.AllocateId();
    const auto bits = state.module.AllocateId();
    const auto absBits = state.module.AllocateId();
    const auto large = state.module.AllocateId();
    const auto finite = state.module.AllocateId();
    const auto largeFinite = state.module.AllocateId();
    const auto reduced = state.module.AllocateId();
    state.module.AddFunction(spv::OpExtInst, TypeF32(state), fract, GlslStd450(state), GLSLstd450Fract, src);
    state.module.AddFunction(spv::OpBitcast, TypeU32(state), bits, src);
    state.module.AddFunction(spv::OpBitwiseAnd, TypeU32(state), absBits, bits, ConstantU32(state, 0x7fffffffu));
    state.module.AddFunction(spv::OpUGreaterThanEqual, TypeBool(state), large, absBits, ConstantU32(state, 0x4b000000u));
    state.module.AddFunction(spv::OpULessThan, TypeBool(state), finite, absBits, ConstantU32(state, 0x7f800000u));
    state.module.AddFunction(spv::OpLogicalAnd, TypeBool(state), largeFinite, large, finite);
    state.module.AddFunction(spv::OpSelect, TypeF32(state), reduced, largeFinite, ConstantF32Value(state, 0.0f), fract);
    if (!preserveSignedZero) {
        return reduced;
    }
    const auto zero = state.module.AllocateId();
    const auto result = state.module.AllocateId();
    state.module.AddFunction(spv::OpIEqual, TypeBool(state), zero, absBits, ConstantU32(state, 0u));
    state.module.AddFunction(spv::OpSelect, TypeF32(state), result, zero, src, reduced);
    return result;
}

std::uint32_t EmitF16BitsToF32(SpirvEmitterState& state, std::uint32_t bits) {
    const auto unpacked = state.module.AllocateId();
    const auto result = state.module.AllocateId();
    state.module.AddFunction(spv::OpExtInst, TypeF32Vector(state, 2), unpacked, GlslStd450(state), GLSLstd450UnpackHalf2x16, bits);
    state.module.AddFunction(spv::OpCompositeExtract, TypeF32(state), result, unpacked, 0u);
    return result;
}

void EmitProgram(SpirvEmitterState& state) {
    throw std::runtime_error("EmitProgram not implemented");
}

void DefineGetBdaPointer(SpirvEmitterState& state) {
    throw std::runtime_error("DefineGetBdaPointer not implemented");
}

void EmitLabel(SpirvEmitterState& state, std::uint32_t label) {
    state.currentLabel = label;
    state.module.AddFunction(spv::OpLabel, label);
}

std::uint32_t Unary(SpirvEmitterState& state, std::uint32_t opcode, std::uint32_t type, std::uint32_t value) {
    const auto result = state.module.AllocateId();
    state.module.AddFunction(opcode, type, result, value);
    return result;
}

std::uint32_t Binary(SpirvEmitterState& state, std::uint32_t opcode, std::uint32_t type, std::uint32_t lhs, std::uint32_t rhs) {
    const auto result = state.module.AllocateId();
    state.module.AddFunction(opcode, type, result, lhs, rhs);
    return result;
}

std::uint32_t Select(SpirvEmitterState& state, std::uint32_t type, std::uint32_t condition, std::uint32_t trueValue, std::uint32_t falseValue) {
    const auto result = state.module.AllocateId();
    state.module.AddFunction(spv::OpSelect, type, result, condition, trueValue, falseValue);
    return result;
}

std::uint32_t EmitMeshDrawParameter(SpirvValueEmitContext& ctx, const IrValue& inst) {
    throw std::runtime_error("EmitMeshDrawParameter not implemented");
}

std::uint32_t EmitGetTessellationAttribute(SpirvValueEmitContext& ctx, const IrValue& inst) {
    throw std::runtime_error("EmitGetTessellationAttribute not implemented");
}

void EmitSetTessellationAttribute(SpirvValueEmitContext& ctx, const IrValue& inst) {
    throw std::runtime_error("EmitSetTessellationAttribute not implemented");
}

std::uint32_t EmitGetUserData(SpirvEmitterState& state, ScalarReg reg) {
    throw std::runtime_error("EmitGetUserData not implemented");
}

std::uint32_t EmitGetBuiltin(SpirvValueEmitContext& ctx, const IrValue* kind, const IrValue* index) {
    throw std::runtime_error("EmitGetBuiltin not implemented");
}

std::uint32_t EmitGetAttribute(SpirvValueEmitContext& ctx, const IrValue& inst) {
    throw std::runtime_error("EmitGetAttribute not implemented");
}

std::uint32_t EmitGetInterpolationParameter(SpirvValueEmitContext& ctx, const IrValue& inst) {
    throw std::runtime_error("EmitGetInterpolationParameter not implemented");
}

void EmitSetAttribute(SpirvValueEmitContext& ctx, const IrValue& inst) {
    throw std::runtime_error("EmitSetAttribute not implemented");
}

std::uint32_t EmitGetShaderBase(SpirvValueEmitContext& ctx) {
    throw std::runtime_error("EmitGetShaderBase not implemented");
}

void EmitTessellationBase(SpirvValueEmitContext& ctx, const IrValue& inst) {
    throw std::runtime_error("EmitTessellationBase not implemented");
}

}