function(add_shader_recompiler target runtimeTarget)
    if(NOT TARGET "${runtimeTarget}")
        message(FATAL_ERROR "Shader recompiler requires an existing runtime target: ${runtimeTarget}")
    endif()
    add_library(${target} STATIC EXCLUDE_FROM_ALL
        ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/Recompiler.cpp
        ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/RdnaDecoder/src/RdnaOpcode.cpp
        ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/RdnaDecoder/src/RdnaProgram.cpp
        ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/RdnaDecoder/src/RdnaInstructionDecoder.cpp
        ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/RdnaDecoder/src/RdnaScalarOpDecoder.cpp
        ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/RdnaDecoder/src/RdnaVectorOpDecoder.cpp
        ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/RdnaDecoder/src/RdnaMemoryOpDecoder.cpp
        ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/RdnaDecoder/src/RdnaImageOpDecoder.cpp
        ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/RdnaDecoder/src/RdnaExportOpDecoder.cpp
        ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/ControlFlow/src/ControlFlowGraph.cpp
        ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/ControlFlow/src/GraphBuilder.cpp
        ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/ControlFlow/src/Structurizer.cpp
        ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/IntermediateRepresentation/src/IrType.cpp
        ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/IntermediateRepresentation/src/GuestRegister.cpp
        ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/IntermediateRepresentation/src/IrOpcode.cpp
        ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/IntermediateRepresentation/src/IrValue.cpp
        ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/IntermediateRepresentation/src/IrBlock.cpp
        ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/IntermediateRepresentation/src/IrProgram.cpp
        ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/IntermediateRepresentation/src/IrBuilder/IrBuilderInternal.cpp
        ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/IntermediateRepresentation/src/IrBuilder/IrBuilderCore.cpp
        ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/IntermediateRepresentation/src/IrBuilder/IrBuilderConstants.cpp
        ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/IntermediateRepresentation/src/IrBuilder/IrBuilderRegisters.cpp
        ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/IntermediateRepresentation/src/IrBuilder/IrBuilderControlFlow.cpp
        ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/IntermediateRepresentation/src/IrBuilder/IrBuilderExtendedRegisters.cpp
        ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/IntermediateRepresentation/src/IrBuilder/IrBuilderConversions.cpp
        ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/IntermediateRepresentation/src/IrBuilder/IrBuilderArithmetic.cpp
        ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/IntermediateRepresentation/src/IrBuilder/IrBuilderLogic.cpp
        ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/Translation/src/EmbeddedVertexFetch.cpp
        ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/Translation/src/InstructionTranslator.cpp
        ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/Translation/src/ShaderInputInfoBuilder.cpp
        ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/Translation/src/TranslationContext/OperandAccess.cpp
        ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/Translation/src/TranslationContext/SubwordPacking.cpp
        ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/Translation/src/TranslationContext/ExecMask.cpp
        ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/Translation/src/ScalarInstructions.cpp
        ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/Translation/src/VectorInstructions.cpp
        ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/Translation/src/FloatInstructions.cpp
        ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/Translation/src/IntegerInstructions.cpp
        ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/Translation/src/CompareInstructions.cpp
        ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/Translation/src/ControlFlowInstructions.cpp
        ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/Translation/src/ConvertInstructions.cpp
        ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/Translation/src/MemoryInstructions/MemoryResourceHelpers.cpp
        ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/Translation/src/MemoryInstructions/ScalarBufferInstructions.cpp
        ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/Translation/src/MemoryInstructions/BufferInstructions.cpp
        ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/Translation/src/MemoryInstructions/FlatInstructions.cpp
        ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/Translation/src/MemoryInstructions/ImageInstructions.cpp
        ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/Translation/src/MemoryInstructions/SharedMemoryInstructions.cpp
        ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/Translation/src/MemoryInstructions/MemoryInstructionDispatch.cpp
        ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/Translation/src/AttributeInstructions.cpp
        ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/Translation/src/DispatchInstructions.cpp
        ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/Optimization/src/SsaBuilder.cpp
        ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/Optimization/src/ConstantFolder.cpp
        ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/Optimization/src/DeadCodeEliminator.cpp
        ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/Optimization/src/ReadLaneEliminator.cpp
        ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/Optimization/src/ResourceTracker.cpp
        ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/Optimization/src/ResourceMaterializer.cpp
        ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/Optimization/src/BindingAllocator.cpp
        ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/Optimization/src/ShaderInfoCollector.cpp
        ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/Optimization/src/SharedMemoryBarrierInserter.cpp
        ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/Optimization/src/SrtWalker.cpp
        ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/SpirvBackend/src/SpirvModule.cpp
        ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/SpirvBackend/src/SpirvAnalysis.cpp
        ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/SpirvBackend/src/SpirvAluEmitter.cpp
        ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/SpirvBackend/src/SpirvFlowEmitter.cpp
        ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/SpirvBackend/src/SpirvImageEmitter.cpp
        ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/SpirvBackend/src/SpirvMemoryEmitter.cpp
        ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/SpirvBackend/src/SpirvModuleEmitter.cpp
        ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/SpirvBackend/src/SpirvEmitter.cpp
    )
    target_include_directories(${target} PUBLIC
        ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/
        ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/RdnaDecoder/include
        ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/ControlFlow/include
        ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/IntermediateRepresentation/include
        ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/Translation/include
        ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/Optimization/include
        ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/SpirvBackend/include
    )

    target_compile_features(${target} PUBLIC cxx_std_20)
    set_target_properties(${target} PROPERTIES POSITION_INDEPENDENT_CODE ON CXX_EXTENSIONS OFF)
    target_link_libraries(${target} PRIVATE ${runtimeTarget})
endfunction()
