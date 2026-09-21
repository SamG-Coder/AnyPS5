#include "SpirvBackend/SpirvEmitterInstructions.hpp"
#include "SpirvBackend/SpirvBufferFormat.hpp"
#include <spirv/unified1/spirv.hpp>
#include <algorithm>
#include <array>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <vector>

namespace ShaderRecompiler {
namespace {

[[noreturn]] void ThrowNotImplemented(const char* functionName) {
    throw std::runtime_error(std::string("ShaderRecompiler::") + functionName + " not implemented");
}

std::uint32_t AndCondition(SpirvEmitterState& state, std::uint32_t lhs, std::uint32_t rhs) {
    return Binary(state, spv::OpLogicalAnd, TypeBool(state), lhs, rhs);
}

std::uint32_t BufferByteAddress(SpirvValueEmitContext& ctx, const IrValue& inst, const MemoryInfo& mem, std::uint32_t index, std::uint32_t offset, std::uint32_t soffset) {
    auto& state = ctx.state;
    const std::uint32_t packed = StorageBufferPackedStride(state, mem);
    const std::uint32_t stride = packed & 0x3fffu;
    const bool swizzle = stride != 0u && ((packed >> 14u) & 1u) != 0u;
    if (((packed >> 20u) & 1u) != 0u) {
        const auto lane = Binary(state, spv::OpBitwiseAnd, TypeU32(state), EmitSubgroupLocalInvocationId(state), ConstantU32(state, 63u));
        index = Binary(state, spv::OpIAdd, TypeU32(state), index, lane);
    }
    if (mem.offset != 0u) {
        offset = Binary(state, spv::OpIAdd, TypeU32(state), offset, ConstantU32(state, mem.offset));
    }
    std::uint32_t address = 0;
    if (!swizzle) {
        if (stride == 0u) {
            address = offset;
        } else {
            const auto indexed = stride == 1u ? index : Binary(state, spv::OpIMul, TypeU32(state), index, ConstantU32(state, stride));
            address = Binary(state, spv::OpIAdd, TypeU32(state), indexed, offset);
        }
    } else {
        const std::uint32_t strideEnum = (packed >> 16u) & 3u;
        const std::uint32_t indexStride = 8u << strideEnum;
        const auto indexMsb = Binary(state, spv::OpShiftRightLogical, TypeU32(state), index, ConstantU32(state, strideEnum + 3u));
        const auto indexLsb = Binary(state, spv::OpBitwiseAnd, TypeU32(state), index, ConstantU32(state, indexStride - 1u));
        const auto offsetMsb = Binary(state, spv::OpBitwiseAnd, TypeU32(state), offset, ConstantU32(state, ~3u));
        const auto offsetLsb = Binary(state, spv::OpBitwiseAnd, TypeU32(state), offset, ConstantU32(state, 3u));
        const auto indexedMsb = stride == 1u ? indexMsb : Binary(state, spv::OpIMul, TypeU32(state), indexMsb, ConstantU32(state, stride));
        const auto msb = Binary(state, spv::OpIMul, TypeU32(state), Binary(state, spv::OpIAdd, TypeU32(state), indexedMsb, offsetMsb), ConstantU32(state, indexStride));
        const auto lsb = Binary(state, spv::OpIAdd, TypeU32(state), Binary(state, spv::OpShiftLeftLogical, TypeU32(state), indexLsb, ConstantU32(state, 2u)), offsetLsb);
        address = Binary(state, spv::OpIAdd, TypeU32(state), msb, lsb);
    }
    const IrValue* soffsetValue = inst.Argument(3)->Resolve();
    if (soffsetValue->HasImmediate() && soffsetValue->Type() == IrType::U32 && soffsetValue->ImmediateU32() == 0u) {
        return address;
    }
    return Binary(state, spv::OpIAdd, TypeU32(state), address, soffset);
}

struct WordPair {
    std::uint32_t low = 0;
    std::uint32_t high = 0;
};

WordPair AddWordPair(SpirvEmitterState& state, const WordPair& value, const WordPair& addend) {
    const auto low = Binary(state, spv::OpIAdd, TypeU32(state), value.low, addend.low);
    const auto carry = Binary(state, spv::OpULessThan, TypeBool(state), low, value.low);
    const auto carryValue = Select(state, TypeU32(state), carry, ConstantU32(state, 1u), ConstantU32(state, 0u));
    const auto high = Binary(state, spv::OpIAdd, TypeU32(state), Binary(state, spv::OpIAdd, TypeU32(state), value.high, addend.high), carryValue);
    return {low, high};
}

std::uint32_t ScratchByteAddress(SpirvValueEmitContext& ctx, const MemoryInfo& mem, std::uint32_t low, std::uint32_t high) {
    auto& state = ctx.state;
    const auto immediate = static_cast<std::int32_t>(mem.offset);
    const WordPair addend{ConstantU32(state, static_cast<std::uint32_t>(immediate)), ConstantU32(state, immediate < 0 ? 0xffffffffu : 0u)};
    const auto sum = AddWordPair(state, {low, high}, addend);
    const auto valid = Binary(state, spv::OpIEqual, TypeBool(state), sum.high, ConstantU32(state, 0u));
    return Select(state, TypeU32(state), valid, sum.low, ConstantU32(state, 0xffffffffu));
}

std::uint32_t ByteAddress(SpirvValueEmitContext& ctx, const IrValue& inst, const MemoryInfo& mem) {
    auto& state = ctx.state;
    switch (mem.kind) {
    case ResourceKind::Buffer:
        return BufferByteAddress(ctx, inst, mem, ctx.Arg(inst, 1), ctx.Arg(inst, 2), ctx.Arg(inst, 3));
    case ResourceKind::Lds:
    case ResourceKind::Gds:
        if (mem.offset == 0u) {
            return ctx.Arg(inst, 0);
        }
        return Binary(state, spv::OpIAdd, TypeU32(state), ctx.Arg(inst, 0), ConstantU32(state, mem.offset));
    case ResourceKind::Scratch:
        return ScratchByteAddress(ctx, mem, ctx.Arg(inst, 1), ctx.Arg(inst, 2));
    default:
        ctx.Fail(inst, "has a memory kind without a dword-addressed emitter");
    }
}

std::uint32_t DwordIndex(SpirvValueEmitContext& ctx, const IrValue& inst, const MemoryInfo& mem) {
    return Binary(ctx.state, spv::OpShiftRightLogical, TypeU32(ctx.state), ByteAddress(ctx, inst, mem), ConstantU32(ctx.state, 2u));
}

std::uint32_t ActiveArgument(SpirvValueEmitContext& ctx, const IrValue& inst) {
    return ctx.Arg(inst, inst.ArgumentCount() - 1u);
}

std::uint32_t LoadWordInBounds(SpirvValueEmitContext& ctx, const MemoryResourceAccess& resource, std::uint32_t index) {
    auto& state = ctx.state;
    const auto pointer = EmitMemoryElementPointer(state, resource, index);
    const auto value = state.module.AllocateId();
    state.module.AddFunction(spv::OpLoad, TypeU32(state), value, pointer);
    return value;
}

std::uint32_t LoadSubwordInBounds(SpirvValueEmitContext& ctx, const MemoryResourceAccess& resource, std::uint32_t address, std::uint32_t index, std::uint32_t bits, bool signExtend) {
    auto& state = ctx.state;
    if (bits != 8u && bits != 16u) {
        ctx.Fail("subword load has an unsupported width");
    }
    const auto word = LoadWordInBounds(ctx, resource, index);
    const auto byte = Binary(state, spv::OpBitwiseAnd, TypeU32(state), address, ConstantU32(state, 3u));
    const auto shift = Binary(state, spv::OpShiftLeftLogical, TypeU32(state), byte, ConstantU32(state, 3u));
    const auto value = Binary(state, spv::OpBitwiseAnd, TypeU32(state), Binary(state, spv::OpShiftRightLogical, TypeU32(state), word, shift), ConstantU32(state, bits == 8u ? 0xffu : 0xffffu));
    if (!signExtend) {
        return value;
    }
    const auto left = Binary(state, spv::OpShiftLeftLogical, TypeU32(state), value, ConstantU32(state, 32u - bits));
    return Binary(state, spv::OpShiftRightArithmetic, TypeU32(state), left, ConstantU32(state, 32u - bits));
}

std::uint32_t LoadWordPrepared(SpirvValueEmitContext& ctx, const IrValue& inst, const MemoryInfo& mem, const MemoryResourceAccess& resource) {
    auto& state = ctx.state;
    const auto index = EmitMemoryElementIndex(state, resource, DwordIndex(ctx, inst, mem));
    return EmitValueOrZeroIfCondition(state, EmitMemoryElementInBounds(state, resource, index), [&]() {
        return LoadWordInBounds(ctx, resource, index);
    });
}

std::uint32_t LoadWord(SpirvValueEmitContext& ctx, const IrValue& inst, const MemoryInfo& mem) {
    return EmitValueOrZeroIfCondition(ctx.state, ActiveArgument(ctx, inst), [&]() {
        const auto resource = PrepareMemoryResourceAccess(ctx.state, mem);
        return LoadWordPrepared(ctx, inst, mem, resource);
    });
}

std::uint32_t LoadSubwordPrepared(SpirvValueEmitContext& ctx, const IrValue& inst, const MemoryInfo& mem, const MemoryResourceAccess& resource, std::uint32_t bits, bool signExtend) {
    auto& state = ctx.state;
    const auto address = ByteAddress(ctx, inst, mem);
    const auto rawIndex = Binary(state, spv::OpShiftRightLogical, TypeU32(state), address, ConstantU32(state, 2u));
    const auto index = EmitMemoryElementIndex(state, resource, rawIndex);
    return EmitValueOrZeroIfCondition(state, EmitMemoryElementInBounds(state, resource, index), [&]() {
        return LoadSubwordInBounds(ctx, resource, address, index, bits, signExtend);
    });
}

std::uint32_t LoadSubword(SpirvValueEmitContext& ctx, const IrValue& inst, const MemoryInfo& mem, std::uint32_t bits) {
    return EmitValueOrZeroIfCondition(ctx.state, ActiveArgument(ctx, inst), [&]() {
        const auto resource = PrepareMemoryResourceAccess(ctx.state, mem);
        return LoadSubwordPrepared(ctx, inst, mem, resource, bits, false);
    });
}

std::uint32_t ConstantDeviceAddress(SpirvEmitterState& state, std::uint64_t value) {
    return state.module.Constant(spv::OpConstant, TypeScalarU64(state), static_cast<std::uint32_t>(value), static_cast<std::uint32_t>(value >> 32u));
}

std::uint32_t DeviceAddressFromWords(SpirvEmitterState& state, std::uint32_t low, std::uint32_t high) {
    const auto low64 = Unary(state, spv::OpUConvert, TypeScalarU64(state), low);
    const auto high64 = Binary(state, spv::OpShiftLeftLogical, TypeScalarU64(state), Unary(state, spv::OpUConvert, TypeScalarU64(state), high), ConstantDeviceAddress(state, 32u));
    return Binary(state, spv::OpBitwiseOr, TypeScalarU64(state), low64, high64);
}

std::uint32_t GuestAddress(SpirvValueEmitContext& ctx, const IrValue& inst, const MemoryInfo& mem) {
    auto& state = ctx.state;
    auto low = ctx.Arg(inst, 1);
    if (mem.kind == ResourceKind::ScalarAddress) {
        low = Binary(state, spv::OpBitwiseAnd, TypeU32(state), low, ConstantU32(state, ~3u));
    }
    std::uint32_t address = 0;
    if (mem.addressIsFull) {
        address = DeviceAddressFromWords(state, low, ctx.Arg(inst, 2));
    } else {
        const IrValue* argument = inst.Argument(0);
        const IrValue* handle = argument != nullptr ? argument->Resolve() : nullptr;
        if (handle == nullptr || handle->Opcode() != IrOpcode::GetAddressResource || handle->ArgumentCount() != 2u) {
            ctx.Fail(inst, "has no address base pair");
        }
        const auto base = DeviceAddressFromWords(state, ctx.Arg(*handle, 0), ctx.Arg(*handle, 1));
        address = Binary(state, spv::OpIAdd, TypeScalarU64(state), base, Unary(state, spv::OpUConvert, TypeScalarU64(state), low));
    }
    auto immediate = static_cast<std::int32_t>(mem.offset);
    if (mem.kind == ResourceKind::ScalarAddress) {
        immediate = static_cast<std::int32_t>(static_cast<std::uint32_t>(immediate) & ~3u);
    }
    if (immediate == 0) {
        return address;
    }
    return Binary(state, spv::OpIAdd, TypeScalarU64(state), address, ConstantDeviceAddress(state, static_cast<std::uint64_t>(static_cast<std::int64_t>(immediate))));
}

std::uint32_t GetBdaPointer(SpirvValueEmitContext& ctx, std::uint32_t address) {
    auto& state = ctx.state;
    if (state.bdaPointerFunction == 0u) {
        ctx.Fail("BDA pointer function was not defined before function emission");
    }
    const auto result = state.module.AllocateId();
    state.module.AddFunction(spv::OpFunctionCall, TypeScalarU64(state), result, state.bdaPointerFunction, address);
    return result;
}

std::uint32_t LoadBdaDword(SpirvValueEmitContext& ctx, std::uint32_t address) {
    auto& state = ctx.state;
    const auto bda = GetBdaPointer(ctx, address);
    const auto present = Binary(state, spv::OpINotEqual, TypeBool(state), bda, ConstantDeviceAddress(state, 0u));
    return EmitValueOrZeroIfCondition(state, present, [&]() {
        const auto pointer = state.module.AllocateId();
        state.module.AddFunction(spv::OpConvertUToPtr, TypePhysicalU32Pointer(state), pointer, bda);
        const auto value = state.module.AllocateId();
        state.module.AddFunction(spv::OpLoad, TypeU32(state), value, pointer, spv::MemoryAccessAlignedMask, static_cast<std::uint32_t>(sizeof(std::uint32_t)));
        return value;
    });
}

std::uint32_t LoadBda(SpirvValueEmitContext& ctx, const IrValue& inst, const MemoryInfo& mem, std::uint32_t bits) {
    auto& state = ctx.state;
    if (bits != 8u && bits != 16u && bits != 32u) {
        ctx.Fail(inst, "has an unsupported address load width");
    }
    const auto address = GuestAddress(ctx, inst, mem);
    return EmitValueOrZeroIfCondition(state, ActiveArgument(ctx, inst), [&]() {
        const auto aligned = Binary(state, spv::OpBitwiseAnd, TypeScalarU64(state), address, ConstantDeviceAddress(state, ~std::uint64_t{3}));
        const auto first = LoadBdaDword(ctx, aligned);
        const auto byte = Binary(state, spv::OpBitwiseAnd, TypeU32(state), Unary(state, spv::OpUConvert, TypeU32(state), address), ConstantU32(state, 3u));
        const auto crosses = bits == 8u ? ConstantBool(state, false) : Binary(state, bits == 16u ? spv::OpUGreaterThan : spv::OpINotEqual, TypeBool(state), byte, ConstantU32(state, bits == 16u ? 2u : 0u));
        const auto second = EmitValueOrZeroIfCondition(state, crosses, [&]() {
            return LoadBdaDword(ctx, Binary(state, spv::OpIAdd, TypeScalarU64(state), aligned, ConstantDeviceAddress(state, sizeof(std::uint32_t))));
        });
        const auto shift = Binary(state, spv::OpShiftLeftLogical, TypeU32(state), byte, ConstantU32(state, 3u));
        const auto upperByte = Binary(state, spv::OpBitwiseAnd, TypeU32(state), Binary(state, spv::OpISub, TypeU32(state), ConstantU32(state, 4u), byte), ConstantU32(state, 3u));
        const auto upperShift = Binary(state, spv::OpShiftLeftLogical, TypeU32(state), upperByte, ConstantU32(state, 3u));
        const auto merged = Binary(state, spv::OpBitwiseOr, TypeU32(state), Binary(state, spv::OpShiftRightLogical, TypeU32(state), first, shift), Binary(state, spv::OpShiftLeftLogical, TypeU32(state), second, upperShift));
        if (bits == 32u) {
            return merged;
        }
        return Binary(state, spv::OpBitwiseAnd, TypeU32(state), merged, ConstantU32(state, bits == 8u ? 0xffu : 0xffffu));
    });
}

IrBufferFormat BufferFormatOf(const SpirvEmitterState& state, const MemoryInfo& mem) {
    return mem.typed ? DecodeTBufferFormat(mem.dataFormat, mem.numberFormat) : StorageBufferFormat(state, mem);
}

SpirvBufferFormatInfo MemoryFormatInfo(const SpirvEmitterState& state, const MemoryInfo& mem) {
    return GetFormatInfo(mem.formatted ? BufferFormatOf(state, mem) : IrBufferFormat::Invalid);
}

MemoryInfo RebaseFormattedComponent(MemoryInfo mem, const SpirvBufferFormatInfo& info, std::uint32_t component) {
    mem.offset += GetFormatComponentByteOffset(info, component);
    mem.dataDwords = 1u;
    mem.componentIndex = component;
    return mem;
}

MemoryInfo RebaseRawComponent(MemoryInfo mem, std::uint32_t component) {
    mem.offset += component * 4u;
    mem.dataDwords = 1u;
    mem.componentIndex = component;
    return mem;
}

SpirvFormattedSource ResolveOutputSource(SpirvValueEmitContext& ctx, const MemoryInfo& mem, const SpirvBufferFormatInfo& info, std::uint32_t outputComponent) {
    if (mem.typed) {
        if (outputComponent < info.componentCount) {
            return {SpirvFormattedSourceKind::Memory, outputComponent};
        }
        return {};
    }
    const auto swizzle = ctx.state.program.Info().buffers.at(mem.resource).descriptorSwizzle;
    return ResolveFormattedSource(info, (swizzle >> (outputComponent * 3u)) & 0x7u);
}

std::uint32_t FormattedConstant(SpirvValueEmitContext& ctx, const SpirvBufferFormatInfo& info, SpirvFormattedSourceKind kind) {
    return ConstantU32(ctx.state, FormattedConstantBits(info, kind));
}

template<typename TLoadWord, typename TLoadSubword>
std::uint32_t LoadFormattedComponent(SpirvValueEmitContext& ctx, const MemoryInfo& mem, const SpirvBufferFormatInfo& info, std::uint32_t outputComponent, TLoadWord&& loadWord, TLoadSubword&& loadSubword) {
    auto& state = ctx.state;
    const auto source = ResolveOutputSource(ctx, mem, info, outputComponent);
    if (source.kind != SpirvFormattedSourceKind::Memory) {
        return FormattedConstant(ctx, info, source.kind);
    }
    const auto component = source.component;
    const auto bits = info.componentBits[component];
    const bool signedType = IsSignedFormatComponent(info.type);
    std::uint32_t raw = 0;
    if (info.packedBitfield) {
        raw = loadWord(component);
        const auto type = signedType ? TypeI32(state) : TypeU32(state);
        const auto sourceValue = signedType ? Unary(state, spv::OpBitcast, type, raw) : raw;
        const auto extracted = state.module.AllocateId();
        state.module.AddFunction(signedType ? spv::OpBitFieldSExtract : spv::OpBitFieldUExtract, type, extracted, sourceValue, ConstantU32(state, info.componentBitOffset[component]), ConstantU32(state, bits));
        raw = signedType ? Unary(state, spv::OpBitcast, TypeU32(state), extracted) : extracted;
    } else if (bits == 32u) {
        raw = loadWord(component);
    } else {
        raw = loadSubword(component, bits, signedType);
    }
    return NormalizeFormatComponent(state, info, component, raw);
}

std::uint32_t FormattedLoadPrepared(SpirvValueEmitContext& ctx, const IrValue& inst, const MemoryInfo& mem, std::uint32_t outputComponent, const MemoryResourceAccess& resource) {
    const auto info = MemoryFormatInfo(ctx.state, mem);
    if (info.type == SpirvFormatComponentType::Unknown) {
        return LoadWordPrepared(ctx, inst, RebaseRawComponent(mem, outputComponent), resource);
    }
    return LoadFormattedComponent(ctx, mem, info, outputComponent, [&](std::uint32_t component) {
        return LoadWordPrepared(ctx, inst, RebaseFormattedComponent(mem, info, component), resource);
    }, [&](std::uint32_t component, std::uint32_t bits, bool signExtend) {
        return LoadSubwordPrepared(ctx, inst, RebaseFormattedComponent(mem, info, component), resource, bits, signExtend);
    });
}

std::uint32_t FormattedLoad(SpirvValueEmitContext& ctx, const IrValue& inst, const MemoryInfo& mem) {
    return EmitValueOrZeroIfCondition(ctx.state, ActiveArgument(ctx, inst), [&]() {
        const auto resource = PrepareMemoryResourceAccess(ctx.state, mem);
        return FormattedLoadPrepared(ctx, inst, mem, 0u, resource);
    });
}

void StoreWordInBounds(SpirvValueEmitContext& ctx, const MemoryResourceAccess& resource, std::uint32_t index, std::uint32_t data) {
    auto& state = ctx.state;
    state.module.AddFunction(spv::OpStore, EmitMemoryElementPointer(state, resource, index), data);
}

void StoreSubwordInBounds(SpirvValueEmitContext& ctx, const MemoryInfo& mem, const MemoryResourceAccess& resource, std::uint32_t address, std::uint32_t index, std::uint32_t bits, std::uint32_t data) {
    auto& state = ctx.state;
    if (bits != 8u && bits != 16u) {
        ctx.Fail("subword store has an unsupported width");
    }
    const auto pointer = EmitMemoryElementPointer(state, resource, index);
    const auto shift = Binary(state, spv::OpShiftLeftLogical, TypeU32(state), Binary(state, spv::OpBitwiseAnd, TypeU32(state), address, ConstantU32(state, 3u)), ConstantU32(state, 3u));
    const auto fieldMask = ConstantU32(state, bits == 8u ? 0xffu : 0xffffu);
    const auto mask = Binary(state, spv::OpShiftLeftLogical, TypeU32(state), fieldMask, shift);
    const auto value = Binary(state, spv::OpShiftLeftLogical, TypeU32(state), Binary(state, spv::OpBitwiseAnd, TypeU32(state), data, fieldMask), shift);
    const auto merge = [&](std::uint32_t old) {
        return Binary(state, spv::OpBitwiseOr, TypeU32(state), Binary(state, spv::OpBitwiseAnd, TypeU32(state), old, Unary(state, spv::OpNot, TypeU32(state), mask)), value);
    };
    if (mem.kind == ResourceKind::Scratch) {
        const auto old = state.module.AllocateId();
        state.module.AddFunction(spv::OpLoad, TypeU32(state), old, pointer);
        state.module.AddFunction(spv::OpStore, pointer, merge(old));
    } else {
        AtomicUpdate(state, pointer, mem.kind, merge);
    }
}

void StoreSubwordPrepared(SpirvValueEmitContext& ctx, const IrValue& inst, const MemoryInfo& mem, const MemoryResourceAccess& resource, std::uint32_t bits, std::uint32_t data) {
    auto& state = ctx.state;
    const auto address = ByteAddress(ctx, inst, mem);
    const auto rawIndex = Binary(state, spv::OpShiftRightLogical, TypeU32(state), address, ConstantU32(state, 2u));
    const auto index = EmitMemoryElementIndex(state, resource, rawIndex);
    EmitIfCondition(state, EmitMemoryElementInBounds(state, resource, index), [&]() {
        StoreSubwordInBounds(ctx, mem, resource, address, index, bits, data);
    });
}

void StoreSubword(SpirvValueEmitContext& ctx, const IrValue& inst, const MemoryInfo& mem, std::uint32_t bits) {
    EmitIfCondition(ctx.state, ActiveArgument(ctx, inst), [&]() {
        const auto resource = PrepareMemoryResourceAccess(ctx.state, mem);
        StoreSubwordPrepared(ctx, inst, mem, resource, bits, ctx.Arg(inst, inst.ArgumentCount() - 2u));
    });
}

void StoreWordPrepared(SpirvValueEmitContext& ctx, const IrValue& inst, const MemoryInfo& mem, const MemoryResourceAccess& resource, std::uint32_t data) {
    auto& state = ctx.state;
    const auto index = EmitMemoryElementIndex(state, resource, DwordIndex(ctx, inst, mem));
    EmitIfCondition(state, EmitMemoryElementInBounds(state, resource, index), [&]() {
        StoreWordInBounds(ctx, resource, index, data);
    });
}

void StoreWord(SpirvValueEmitContext& ctx, const IrValue& inst, const MemoryInfo& mem) {
    EmitIfCondition(ctx.state, ActiveArgument(ctx, inst), [&]() {
        const auto resource = PrepareMemoryResourceAccess(ctx.state, mem);
        StoreWordPrepared(ctx, inst, mem, resource, ctx.Arg(inst, inst.ArgumentCount() - 2u));
    });
}

void FormattedStorePrepared(SpirvValueEmitContext& ctx, const IrValue& inst, const MemoryInfo& mem, std::uint32_t component, const MemoryResourceAccess& resource, std::uint32_t data) {
    const auto info = MemoryFormatInfo(ctx.state, mem);
    if (info.type == SpirvFormatComponentType::Unknown) {
        StoreWordPrepared(ctx, inst, RebaseRawComponent(mem, component), resource, data);
        return;
    }
    if (component >= info.componentCount) {
        return;
    }
    const auto bits = info.componentBits[component];
    const auto componentMem = RebaseFormattedComponent(mem, info, component);
    if (bits == 8u || bits == 16u) {
        StoreSubwordPrepared(ctx, inst, componentMem, resource, bits, data);
    } else {
        StoreWordPrepared(ctx, inst, componentMem, resource, data);
    }
}

void FormattedStore(SpirvValueEmitContext& ctx, const IrValue& inst, const MemoryInfo& mem) {
    EmitIfCondition(ctx.state, ActiveArgument(ctx, inst), [&]() {
        const auto resource = PrepareMemoryResourceAccess(ctx.state, mem);
        FormattedStorePrepared(ctx, inst, mem, 0u, resource, ctx.Arg(inst, inst.ArgumentCount() - 2u));
    });
}

struct PreparedFormattedMemory {
    SpirvBufferFormatInfo info;
    MemoryResourceAccess resource;
    std::array<std::uint32_t, 4> addresses{};
    std::array<std::uint32_t, 4> indices{};
    std::uint32_t inBounds = 0;
};

enum class FormattedAccess {
    Load,
    Store
};

PreparedFormattedMemory PrepareFormattedMemory(SpirvValueEmitContext& ctx, const IrValue& inst, const MemoryInfo& mem, const MemoryResourceAccess& resource, const SpirvBufferFormatInfo& info, std::uint32_t components, FormattedAccess access) {
    auto& state = ctx.state;
    PreparedFormattedMemory plan;
    plan.info = info;
    plan.resource = resource;
    std::array<bool, 4> required{};
    if (access == FormattedAccess::Load) {
        for (std::uint32_t output = 0; output < components; output++) {
            const auto source = ResolveOutputSource(ctx, mem, plan.info, output);
            if (source.kind == SpirvFormattedSourceKind::Memory) {
                required.at(source.component) = true;
            }
        }
    } else {
        for (std::uint32_t component = 0; component < std::min(components, plan.info.componentCount); component++) {
            required.at(component) = true;
        }
    }
    bool firstBound = true;
    for (std::uint32_t component = 0; component < plan.info.componentCount; component++) {
        if (!required.at(component)) {
            continue;
        }
        const auto byteOffset = GetFormatComponentByteOffset(plan.info, component);
        bool reused = false;
        for (std::uint32_t previous = 0; previous < component; previous++) {
            if (required.at(previous) && GetFormatComponentByteOffset(plan.info, previous) == byteOffset) {
                plan.addresses.at(component) = plan.addresses.at(previous);
                plan.indices.at(component) = plan.indices.at(previous);
                reused = true;
                break;
            }
        }
        if (reused) {
            continue;
        }
        const auto componentMem = RebaseFormattedComponent(mem, plan.info, component);
        plan.addresses.at(component) = ByteAddress(ctx, inst, componentMem);
        const auto rawIndex = Binary(state, spv::OpShiftRightLogical, TypeU32(state), plan.addresses.at(component), ConstantU32(state, 2u));
        plan.indices.at(component) = EmitMemoryElementIndex(state, resource, rawIndex);
        const auto componentBound = EmitMemoryElementInBounds(state, resource, plan.indices.at(component));
        if (firstBound) {
            plan.inBounds = componentBound;
            firstBound = false;
        } else {
            plan.inBounds = AndCondition(state, plan.inBounds, componentBound);
        }
    }
    if (firstBound) {
        plan.inBounds = ConstantBool(state, true);
    }
    return plan;
}

std::uint32_t LoadFormattedInBounds(SpirvValueEmitContext& ctx, const MemoryInfo& mem, const PreparedFormattedMemory& plan, std::uint32_t outputComponent) {
    return LoadFormattedComponent(ctx, mem, plan.info, outputComponent, [&](std::uint32_t component) {
        return LoadWordInBounds(ctx, plan.resource, plan.indices.at(component));
    }, [&](std::uint32_t component, std::uint32_t bits, bool signExtend) {
        return LoadSubwordInBounds(ctx, plan.resource, plan.addresses.at(component), plan.indices.at(component), bits, signExtend);
    });
}

std::uint32_t ConstructU32Composite(SpirvEmitterState& state, std::uint32_t components, const std::array<std::uint32_t, 4>& values) {
    const auto result = state.module.AllocateId();
    std::vector<std::uint32_t> words{spv::OpCompositeConstruct, TypeU32Composite(state, components), result};
    words.insert(words.end(), values.begin(), values.begin() + components);
    state.module.AddFunction(words);
    return result;
}

std::uint32_t FormattedOutOfBoundsValue(SpirvValueEmitContext& ctx, const MemoryInfo& mem, const PreparedFormattedMemory& plan, std::uint32_t components) {
    auto& state = ctx.state;
    std::array<std::uint32_t, 4> values{};
    for (std::uint32_t component = 0; component < components; component++) {
        const auto source = ResolveOutputSource(ctx, mem, plan.info, component);
        values.at(component) = source.kind == SpirvFormattedSourceKind::Memory ? ConstantU32(state, 0u) : FormattedConstant(ctx, plan.info, source.kind);
    }
    return ConstructU32Composite(state, components, values);
}

void StoreFormattedInBounds(SpirvValueEmitContext& ctx, const MemoryInfo& mem, const PreparedFormattedMemory& plan, std::uint32_t component, std::uint32_t data) {
    if (component >= plan.info.componentCount) {
        return;
    }
    const auto bits = plan.info.componentBits[component];
    if (bits == 8u || bits == 16u) {
        StoreSubwordInBounds(ctx, mem, plan.resource, plan.addresses.at(component), plan.indices.at(component), bits, data);
    } else {
        StoreWordInBounds(ctx, plan.resource, plan.indices.at(component), data);
    }
}

std::uint32_t LoadWideBuffer(SpirvValueEmitContext& ctx, const IrValue& inst, std::uint32_t components) {
    auto& state = ctx.state;
    return EmitValueOrDefaultIfCondition(state, ActiveArgument(ctx, inst), TypeU32Composite(state, components), ConstantU32CompositeZero(state, components), [&]() {
        const auto& mem = ctx.Memory(inst);
        const auto resource = PrepareMemoryResourceAccess(state, mem);
        const auto info = MemoryFormatInfo(state, mem);
        if (info.type != SpirvFormatComponentType::Unknown) {
            const auto plan = PrepareFormattedMemory(ctx, inst, mem, resource, info, components, FormattedAccess::Load);
            return EmitValueOrDefaultIfCondition(state, plan.inBounds, TypeU32Composite(state, components), FormattedOutOfBoundsValue(ctx, mem, plan, components), [&]() {
                std::array<std::uint32_t, 4> values{};
                for (std::uint32_t component = 0; component < components; component++) {
                    values.at(component) = LoadFormattedInBounds(ctx, mem, plan, component);
                }
                return ConstructU32Composite(state, components, values);
            });
        }
        std::array<std::uint32_t, 4> values{};
        for (std::uint32_t component = 0; component < components; component++) {
            values.at(component) = LoadWordPrepared(ctx, inst, RebaseRawComponent(mem, component), resource);
        }
        return ConstructU32Composite(state, components, values);
    });
}

void StoreWideBuffer(SpirvValueEmitContext& ctx, const IrValue& inst, std::uint32_t components) {
    auto& state = ctx.state;
    EmitIfCondition(state, ActiveArgument(ctx, inst), [&]() {
        const auto& mem = ctx.Memory(inst);
        const auto resource = PrepareMemoryResourceAccess(state, mem);
        const auto composite = ctx.Arg(inst, inst.ArgumentCount() - 2u);
        const auto info = MemoryFormatInfo(state, mem);
        if (info.type != SpirvFormatComponentType::Unknown) {
            const auto plan = PrepareFormattedMemory(ctx, inst, mem, resource, info, components, FormattedAccess::Store);
            EmitIfCondition(state, plan.inBounds, [&]() {
                for (std::uint32_t component = 0; component < components; component++) {
                    const auto data = state.module.AllocateId();
                    state.module.AddFunction(spv::OpCompositeExtract, TypeU32(state), data, composite, component);
                    StoreFormattedInBounds(ctx, mem, plan, component, data);
                }
            });
            return;
        }
        for (std::uint32_t component = 0; component < components; component++) {
            const auto data = state.module.AllocateId();
            state.module.AddFunction(spv::OpCompositeExtract, TypeU32(state), data, composite, component);
            StoreWordPrepared(ctx, inst, RebaseRawComponent(mem, component), resource, data);
        }
    });
}

std::uint32_t LoadWideShared(SpirvValueEmitContext& ctx, const IrValue& inst, std::uint32_t components) {
    auto& state = ctx.state;
    return EmitValueOrDefaultIfCondition(state, ActiveArgument(ctx, inst), TypeU32Composite(state, components), ConstantU32CompositeZero(state, components), [&]() {
        const auto& mem = ctx.Memory(inst);
        const auto resource = PrepareMemoryResourceAccess(state, mem);
        const auto base = ByteAddress(ctx, inst, mem);
        std::array<std::uint32_t, 4> values{};
        for (std::uint32_t component = 0; component < components; component++) {
            const auto address = component == 0u ? base : Binary(state, spv::OpIAdd, TypeU32(state), base, ConstantU32(state, component * 4u));
            const auto rawIndex = Binary(state, spv::OpShiftRightLogical, TypeU32(state), address, ConstantU32(state, 2u));
            const auto index = EmitMemoryElementIndex(state, resource, rawIndex);
            values.at(component) = EmitValueOrZeroIfCondition(state, EmitMemoryElementInBounds(state, resource, index), [&]() {
                return LoadWordInBounds(ctx, resource, index);
            });
        }
        return ConstructU32Composite(state, components, values);
    });
}

void StoreWideShared(SpirvValueEmitContext& ctx, const IrValue& inst, std::uint32_t components) {
    auto& state = ctx.state;
    EmitIfCondition(state, ActiveArgument(ctx, inst), [&]() {
        const auto& mem = ctx.Memory(inst);
        const auto resource = PrepareMemoryResourceAccess(state, mem);
        const auto base = ByteAddress(ctx, inst, mem);
        for (std::uint32_t component = 0; component < components; component++) {
            const auto address = component == 0u ? base : Binary(state, spv::OpIAdd, TypeU32(state), base, ConstantU32(state, component * 4u));
            const auto rawIndex = Binary(state, spv::OpShiftRightLogical, TypeU32(state), address, ConstantU32(state, 2u));
            const auto index = EmitMemoryElementIndex(state, resource, rawIndex);
            EmitIfCondition(state, EmitMemoryElementInBounds(state, resource, index), [&]() {
                StoreWordInBounds(ctx, resource, index, ctx.Arg(inst, component + 1u));
            });
        }
    });
}

}

std::uint32_t EmitReadConst(SpirvValueEmitContext& ctx, const IrValue& inst) {
    auto& state = ctx.state;
    if (state.flattenedSrtVariable == 0) {
        ctx.Fail(inst, "requires the flattened SRT descriptor");
    }
    const auto pointer = state.module.AllocateId();
    state.module.AddFunction(spv::OpAccessChain, TypeStorageBufferElementPointer(state), pointer, state.flattenedSrtVariable, ConstantU32(state, 0u), ctx.Arg(inst, 1));
    const auto value = state.module.AllocateId();
    state.module.AddFunction(spv::OpLoad, TypeU32(state), value, pointer);
    return value;
}

void EmitReadConstBuffer(SpirvValueEmitContext& ctx, const IrValue& inst) {
    const auto& mem = ctx.Memory(inst);
    if (mem.planningOnly) {
        return;
    }
    if (mem.kind != ResourceKind::ScalarBuffer) {
        ctx.Fail(inst, "must read a scalar buffer resource");
    }
    auto& state = ctx.state;
    const auto address = Binary(state, spv::OpIAdd, TypeU32(state), ctx.Arg(inst, 1), ConstantU32(state, mem.offset));
    const auto rawIndex = Binary(state, spv::OpShiftRightLogical, TypeU32(state), address, ConstantU32(state, 2u));
    const auto access = PrepareMemoryResourceAccess(state, mem);
    const auto element = EmitMemoryElementIndex(state, access, rawIndex);
    const auto inBounds = EmitMemoryElementInBounds(state, access, element);
    ctx.Define(inst, EmitValueOrZeroIfCondition(state, inBounds, [&]() {
        const auto value = state.module.AllocateId();
        state.module.AddFunction(spv::OpLoad, TypeU32(state), value, EmitMemoryElementPointer(state, access, element));
        return value;
    }));
}

void EmitGetSrtResource(SpirvValueEmitContext& context) {
    EmitVoid(context);
}

void EmitGetBufferResource(SpirvValueEmitContext& context) {
    EmitVoid(context);
}

void EmitGetAddressResource(SpirvValueEmitContext& context) {
    EmitVoid(context);
}

void EmitGetScratchResource(SpirvValueEmitContext& context) {
    EmitVoid(context);
}

void EmitLoadAddressU8(SpirvValueEmitContext&, const IrValue&) {
    ThrowNotImplemented(__func__);
}

void EmitLoadAddressU16(SpirvValueEmitContext&, const IrValue&) {
    ThrowNotImplemented(__func__);
}

void EmitLoadAddressU32(SpirvValueEmitContext&, const IrValue&) {
    ThrowNotImplemented(__func__);
}

void EmitStoreAddressU8(SpirvValueEmitContext&, const IrValue&) {
    ThrowNotImplemented(__func__);
}

void EmitStoreAddressU16(SpirvValueEmitContext&, const IrValue&) {
    ThrowNotImplemented(__func__);
}

void EmitStoreAddressU32(SpirvValueEmitContext&, const IrValue&) {
    ThrowNotImplemented(__func__);
}

void EmitLoadBufferU8(SpirvValueEmitContext&, const IrValue&) {
    ThrowNotImplemented(__func__);
}

void EmitLoadBufferU16(SpirvValueEmitContext&, const IrValue&) {
    ThrowNotImplemented(__func__);
}

void EmitLoadBufferU32(SpirvValueEmitContext&, const IrValue&) {
    ThrowNotImplemented(__func__);
}

void EmitLoadBufferU32x2(SpirvValueEmitContext&, const IrValue&) {
    ThrowNotImplemented(__func__);
}

void EmitLoadBufferU32x3(SpirvValueEmitContext&, const IrValue&) {
    ThrowNotImplemented(__func__);
}

void EmitLoadBufferU32x4(SpirvValueEmitContext&, const IrValue&) {
    ThrowNotImplemented(__func__);
}

void EmitStoreBufferU8(SpirvValueEmitContext&, const IrValue&) {
    ThrowNotImplemented(__func__);
}

void EmitStoreBufferU16(SpirvValueEmitContext&, const IrValue&) {
    ThrowNotImplemented(__func__);
}

void EmitStoreBufferU32(SpirvValueEmitContext&, const IrValue&) {
    ThrowNotImplemented(__func__);
}

void EmitStoreBufferU32x2(SpirvValueEmitContext&, const IrValue&) {
    ThrowNotImplemented(__func__);
}

void EmitStoreBufferU32x3(SpirvValueEmitContext&, const IrValue&) {
    ThrowNotImplemented(__func__);
}

void EmitStoreBufferU32x4(SpirvValueEmitContext&, const IrValue&) {
    ThrowNotImplemented(__func__);
}

std::uint32_t EmitBufferAtomicSwap32(SpirvValueEmitContext&, const IrValue&) {
    ThrowNotImplemented(__func__);
}

std::uint32_t EmitBufferAtomicCmpSwap32(SpirvValueEmitContext&, const IrValue&) {
    ThrowNotImplemented(__func__);
}

std::uint32_t EmitBufferAtomicSwap64(SpirvValueEmitContext&, const IrValue&) {
    ThrowNotImplemented(__func__);
}

std::uint32_t EmitBufferAtomicIAdd32(SpirvValueEmitContext&, const IrValue&) {
    ThrowNotImplemented(__func__);
}

std::uint32_t EmitBufferAtomicISub32(SpirvValueEmitContext&, const IrValue&) {
    ThrowNotImplemented(__func__);
}

std::uint32_t EmitBufferAtomicSMin32(SpirvValueEmitContext&, const IrValue&) {
    ThrowNotImplemented(__func__);
}

std::uint32_t EmitBufferAtomicUMin32(SpirvValueEmitContext&, const IrValue&) {
    ThrowNotImplemented(__func__);
}

std::uint32_t EmitBufferAtomicSMax32(SpirvValueEmitContext&, const IrValue&) {
    ThrowNotImplemented(__func__);
}

std::uint32_t EmitBufferAtomicUMax32(SpirvValueEmitContext&, const IrValue&) {
    ThrowNotImplemented(__func__);
}

std::uint32_t EmitBufferAtomicAnd32(SpirvValueEmitContext&, const IrValue&) {
    ThrowNotImplemented(__func__);
}

std::uint32_t EmitBufferAtomicOr32(SpirvValueEmitContext&, const IrValue&) {
    ThrowNotImplemented(__func__);
}

std::uint32_t EmitBufferAtomicOr64(SpirvValueEmitContext&, const IrValue&) {
    ThrowNotImplemented(__func__);
}

std::uint32_t EmitBufferAtomicXor32(SpirvValueEmitContext&, const IrValue&) {
    ThrowNotImplemented(__func__);
}

std::uint32_t EmitBufferAtomicFMin32(SpirvValueEmitContext&, const IrValue&) {
    ThrowNotImplemented(__func__);
}

std::uint32_t EmitBufferAtomicFMax32(SpirvValueEmitContext&, const IrValue&) {
    ThrowNotImplemented(__func__);
}

void EmitLoadSharedU8(SpirvValueEmitContext&, const IrValue&) {
    ThrowNotImplemented(__func__);
}

void EmitLoadSharedU16(SpirvValueEmitContext&, const IrValue&) {
    ThrowNotImplemented(__func__);
}

void EmitLoadSharedU32(SpirvValueEmitContext&, const IrValue&) {
    ThrowNotImplemented(__func__);
}

void EmitLoadSharedU32x2(SpirvValueEmitContext&, const IrValue&) {
    ThrowNotImplemented(__func__);
}

void EmitLoadSharedU32x3(SpirvValueEmitContext&, const IrValue&) {
    ThrowNotImplemented(__func__);
}

void EmitLoadSharedU32x4(SpirvValueEmitContext&, const IrValue&) {
    ThrowNotImplemented(__func__);
}

void EmitWriteSharedU8(SpirvValueEmitContext&, const IrValue&) {
    ThrowNotImplemented(__func__);
}

void EmitWriteSharedU16(SpirvValueEmitContext&, const IrValue&) {
    ThrowNotImplemented(__func__);
}

void EmitWriteSharedU32(SpirvValueEmitContext&, const IrValue&) {
    ThrowNotImplemented(__func__);
}

void EmitWriteSharedU32x2(SpirvValueEmitContext&, const IrValue&) {
    ThrowNotImplemented(__func__);
}

void EmitWriteSharedU32x3(SpirvValueEmitContext&, const IrValue&) {
    ThrowNotImplemented(__func__);
}

void EmitWriteSharedU32x4(SpirvValueEmitContext&, const IrValue&) {
    ThrowNotImplemented(__func__);
}

void EmitSharedAtomicFMin32(SpirvValueEmitContext&, const IrValue&) {
    ThrowNotImplemented(__func__);
}

void EmitSharedAtomicFMax32(SpirvValueEmitContext&, const IrValue&) {
    ThrowNotImplemented(__func__);
}

std::uint32_t EmitSharedAtomicSwap32(SpirvValueEmitContext&, const IrValue&) {
    ThrowNotImplemented(__func__);
}

std::uint32_t EmitSharedAtomicIAdd32(SpirvValueEmitContext&, const IrValue&) {
    ThrowNotImplemented(__func__);
}

std::uint32_t EmitSharedAtomicISub32(SpirvValueEmitContext&, const IrValue&) {
    ThrowNotImplemented(__func__);
}

std::uint32_t EmitSharedAtomicInc32(SpirvValueEmitContext&, const IrValue&) {
    ThrowNotImplemented(__func__);
}

std::uint32_t EmitSharedAtomicDec32(SpirvValueEmitContext&, const IrValue&) {
    ThrowNotImplemented(__func__);
}

std::uint32_t EmitSharedAtomicSMin32(SpirvValueEmitContext&, const IrValue&) {
    ThrowNotImplemented(__func__);
}

std::uint32_t EmitSharedAtomicUMin32(SpirvValueEmitContext&, const IrValue&) {
    ThrowNotImplemented(__func__);
}

std::uint32_t EmitSharedAtomicSMax32(SpirvValueEmitContext&, const IrValue&) {
    ThrowNotImplemented(__func__);
}

std::uint32_t EmitSharedAtomicUMax32(SpirvValueEmitContext&, const IrValue&) {
    ThrowNotImplemented(__func__);
}

std::uint32_t EmitSharedAtomicAnd32(SpirvValueEmitContext&, const IrValue&) {
    ThrowNotImplemented(__func__);
}

std::uint32_t EmitSharedAtomicOr32(SpirvValueEmitContext&, const IrValue&) {
    ThrowNotImplemented(__func__);
}

std::uint32_t EmitSharedAtomicXor32(SpirvValueEmitContext&, const IrValue&) {
    ThrowNotImplemented(__func__);
}

std::uint32_t EmitDataAppend(SpirvValueEmitContext&, const IrValue&) {
    ThrowNotImplemented(__func__);
}

std::uint32_t EmitDataConsume(SpirvValueEmitContext&, const IrValue&) {
    ThrowNotImplemented(__func__);
}

}
