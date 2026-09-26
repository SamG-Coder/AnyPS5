function(ensure_agc_glslang)
    if(TARGET glslang AND TARGET SPIRV AND TARGET glslang-standalone)
        return()
    endif()

    if(TARGET glslang)
        if(NOT TARGET SPIRV)
            message(FATAL_ERROR "glslang exists but SPIRV target is missing")
        endif()
        if(NOT TARGET glslang-standalone)
            message(FATAL_ERROR "glslang exists but glslang-standalone target is missing")
        endif()
        return()
    endif()

    set(BUILD_SHARED_LIBS OFF)
    set(BUILD_EXTERNAL OFF)
    set(GLSLANG_TESTS OFF)
    set(GLSLANG_ENABLE_INSTALL OFF)
    set(ENABLE_GLSLANG_BINARIES ON)
    set(ENABLE_SPIRV ON)
    set(ENABLE_SPVREMAPPER OFF)
    set(ENABLE_OPT OFF)

    add_subdirectory(
            "${CMAKE_SOURCE_DIR}/3rdparty/glslang"
            "${CMAKE_BINARY_DIR}/3rdparty/glslang"
            EXCLUDE_FROM_ALL
    )
endfunction()

ensure_agc_glslang()

set(agcTextureDetileShaderSource ${CMAKE_CURRENT_SOURCE_DIR}/Graphics/shaders/TextureDetile.comp)
set(agcTextureDetileGeneratedRoot ${CMAKE_CURRENT_BINARY_DIR}/generated)
set(agcTextureDetileGeneratedDir ${agcTextureDetileGeneratedRoot}/prx/libSceAgcDriver/Graphics/shaders)
set(agcTextureDetileShaderSpv ${agcTextureDetileGeneratedDir}/TextureDetile.spv)
set(agcTextureDetileShaderHeader ${agcTextureDetileGeneratedDir}/TextureDetile_spv.h)
add_custom_command(
        OUTPUT "${agcTextureDetileShaderHeader}"
        COMMAND ${CMAKE_COMMAND} -E make_directory "${agcTextureDetileGeneratedDir}"
        COMMAND "$<TARGET_FILE:glslang-standalone>" -V --target-env vulkan1.1 -o "${agcTextureDetileShaderSpv}" "${agcTextureDetileShaderSource}"
        COMMAND ${CMAKE_COMMAND} -DINPUT=${agcTextureDetileShaderSpv} -DOUTPUT=${agcTextureDetileShaderHeader}
        -DSYMBOL=TEXTURE_DETILE_SPV -P "${CMAKE_CURRENT_SOURCE_DIR}/embed_spirv.cmake"
        DEPENDS "${agcTextureDetileShaderSource}" glslang-standalone
        VERBATIM
)

add_library(libSceAgcDriver SHARED EXCLUDE_FROM_ALL
        ${CMAKE_CURRENT_SOURCE_DIR}/Execution/src/DisplayBuffer.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Execution/src/Driver.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Execution/src/NativeAgc.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/../libSceAgc/Shader/src/ShaderUtils.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Execution/src/GuestMemory.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Execution/src/ShaderMemory.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Execution/src/BdaFeatures.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Execution/src/Pm4.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Execution/src/VulkanDevice.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Execution/src/SwapchainState.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Execution/src/AspectFit.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Execution/src/PresentationScaler.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Graphics/src/Draw.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Graphics/src/ColorTargetLayout.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Graphics/src/DepthTargetLayout.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Graphics/src/DepthState.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Graphics/src/DepthSurface.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Graphics/src/DepthMemoryOwnership.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Graphics/src/ColorTargetTransfer.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Graphics/src/Pipeline.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Graphics/src/Resources.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Graphics/src/ShaderInputState.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Graphics/src/ShaderResources.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Graphics/src/ShaderAddressResources.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Graphics/src/GuestBufferMemory.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Graphics/src/BdaResources.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Graphics/src/BufferAddress.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Graphics/src/ShaderValidation.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Graphics/src/State.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Graphics/src/NativeGraphicsState.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Graphics/src/NativeDrawCompiler.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Graphics/src/GuestTextureResource.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Graphics/src/GuestSamplerResource.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Graphics/src/TextureFormat.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Graphics/src/TextureTiling.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Graphics/src/TextureDetiler.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Graphics/src/Texture.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Graphics/src/Sampler.cpp
        ${agcTextureDetileShaderHeader}
        ${CMAKE_CURRENT_SOURCE_DIR}/Eq/src/Event.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Eq/src/Query.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Resource/src/Lifecycle.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Resource/src/Registration.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/State/src/Configuration.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/State/src/Status.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Submit/src/Acb.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Submit/src/CommandBuffer.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Submit/src/Dcb.cpp
)

target_include_directories(libSceAgcDriver PRIVATE
        ${LIBS_INCLUDE_DIR}
        ${agcTextureDetileGeneratedRoot}
        ${CMAKE_SOURCE_DIR}/3rdparty/Vulkan-Headers/include
        ${CMAKE_SOURCE_DIR}/3rdparty/SPIRV-Headers/include
)

add_shader_recompiler(shader_recompiler_agc_driver libc)

target_link_libraries(libSceAgcDriver PRIVATE
        shader_recompiler_agc_driver
        SDL2-static
        libc
)

set_target_properties(libSceAgcDriver PROPERTIES CXX_EXTENSIONS OFF)
configure_windows_unwind(libSceAgcDriver)

foreach(runtimeLinkFlag IN ITEMS -static-libgcc -static-libstdc++ -static)
    string(REPLACE "${runtimeLinkFlag}" "" CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS}")
endforeach()

add_executable(agc_driver_tests EXCLUDE_FROM_ALL
        tests/Submit.cpp
)

target_include_directories(agc_driver_tests PRIVATE
        ${LIBS_INCLUDE_DIR}
)

target_link_libraries(agc_driver_tests PRIVATE
        libSceAgcDriver
        libc
)

set_target_properties(agc_driver_tests PROPERTIES CXX_EXTENSIONS OFF)
configure_windows_unwind(agc_driver_tests)

add_executable(agc_driver_recompiler_tests EXCLUDE_FROM_ALL
        tests/Recompiler.cpp
)

target_include_directories(agc_driver_recompiler_tests PRIVATE
        ${LIBS_INCLUDE_DIR}
)

target_link_libraries(agc_driver_recompiler_tests PRIVATE
        libSceAgcDriver
        libc
)

set_target_properties(agc_driver_recompiler_tests PROPERTIES CXX_EXTENSIONS OFF)
configure_windows_unwind(agc_driver_recompiler_tests)

add_executable(agc_shader_memory_tests EXCLUDE_FROM_ALL
        tests/ShaderMemory.cpp
        Execution/src/ShaderMemory.cpp
        Execution/src/GuestMemory.cpp
)
target_include_directories(agc_shader_memory_tests PRIVATE ${LIBS_INCLUDE_DIR})
target_link_libraries(agc_shader_memory_tests PRIVATE shader_recompiler_agc_driver libc)
set_target_properties(agc_shader_memory_tests PROPERTIES CXX_EXTENSIONS OFF)
configure_windows_unwind(agc_shader_memory_tests)
if(BUILD_TESTING)
    set_target_properties(agc_shader_memory_tests PROPERTIES EXCLUDE_FROM_ALL FALSE)
    add_test(NAME agc_shader_memory COMMAND agc_shader_memory_tests)
    if(WIN32)
        set_tests_properties(agc_shader_memory PROPERTIES ENVIRONMENT_MODIFICATION
                "PATH=path_list_prepend:$<TARGET_FILE_DIR:libc>")
    endif()
endif()

add_executable(agc_driver_flip_tests EXCLUDE_FROM_ALL
        tests/Flip.cpp
)

target_include_directories(agc_driver_flip_tests PRIVATE
        ${LIBS_INCLUDE_DIR}
)

target_link_libraries(agc_driver_flip_tests PRIVATE
        libSceAgcDriver
        libc
)

set_target_properties(agc_driver_flip_tests PROPERTIES CXX_EXTENSIONS OFF)
configure_windows_unwind(agc_driver_flip_tests)

add_executable(agc_driver_pm4_tests EXCLUDE_FROM_ALL
        tests/Pm4.cpp
)

target_include_directories(agc_driver_pm4_tests PRIVATE
        ${LIBS_INCLUDE_DIR}
)

target_link_libraries(agc_driver_pm4_tests PRIVATE
        libSceAgcDriver
        libc
)

set_target_properties(agc_driver_pm4_tests PROPERTIES CXX_EXTENSIONS OFF)
configure_windows_unwind(agc_driver_pm4_tests)

add_executable(agc_driver_graphics_tests EXCLUDE_FROM_ALL
        tests/PixelInputs.cpp
        tests/Graphics.cpp
        tests/ColorTargetLayout.cpp
        tests/BdaResources.cpp
        tests/GuestAllocations.cpp
        tests/TextureFormat.cpp
        tests/TextureTiling.cpp
        tests/GuestTextureResource.cpp
        tests/GuestSamplerResource.cpp
        tests/TextureDetiler.cpp
        Graphics/src/State.cpp
        Graphics/src/DepthState.cpp
        Graphics/src/DepthTargetLayout.cpp
        Graphics/src/DepthSurface.cpp
        Graphics/src/ColorTargetLayout.cpp
        Graphics/src/ColorTargetTransfer.cpp
        Graphics/src/Resources.cpp
        Graphics/src/ShaderResources.cpp
        Graphics/src/ShaderAddressResources.cpp
        Graphics/src/GuestBufferMemory.cpp
        Graphics/src/BdaResources.cpp
        Graphics/src/BufferAddress.cpp
        Graphics/src/ShaderValidation.cpp
        Graphics/src/GuestTextureResource.cpp
        Graphics/src/GuestSamplerResource.cpp
        Graphics/src/TextureFormat.cpp
        Graphics/src/TextureTiling.cpp
        Graphics/src/TextureDetiler.cpp
        Graphics/src/Texture.cpp
        Graphics/src/Sampler.cpp
        ${agcTextureDetileShaderHeader}
        Execution/src/GuestMemory.cpp
)

target_include_directories(agc_driver_graphics_tests PRIVATE
        ${LIBS_INCLUDE_DIR}
        ${agcTextureDetileGeneratedRoot}
        ${CMAKE_SOURCE_DIR}/core/shader/recompiler
        ${CMAKE_SOURCE_DIR}/core/shader/recompiler/IntermediateRepresentation/include
        ${CMAKE_SOURCE_DIR}/core/shader/recompiler/ControlFlow/include
        ${CMAKE_SOURCE_DIR}/core/shader/recompiler/RdnaDecoder/include
        ${CMAKE_SOURCE_DIR}/3rdparty/Vulkan-Headers/include
        ${CMAKE_SOURCE_DIR}/3rdparty/SPIRV-Headers/include
)

target_link_libraries(agc_driver_graphics_tests PRIVATE
        libc
        libSceAgcDriver
        shader_recompiler_agc_driver
)

set_target_properties(agc_driver_graphics_tests PROPERTIES CXX_EXTENSIONS OFF)
configure_windows_unwind(agc_driver_graphics_tests)
if(BUILD_TESTING)
    set_target_properties(agc_driver_graphics_tests PROPERTIES EXCLUDE_FROM_ALL FALSE)
    add_test(NAME agc_shader_stages COMMAND agc_driver_graphics_tests stages)
    add_test(NAME agc_scan_conversion COMMAND agc_driver_graphics_tests scan)
    add_test(NAME agc_clipping_state COMMAND agc_driver_graphics_tests clipping)
    add_test(NAME agc_provoking_vertex COMMAND agc_driver_graphics_tests provoking)
    add_test(NAME agc_color_control COMMAND agc_driver_graphics_tests color-control)
    add_test(NAME agc_shader_headers COMMAND agc_driver_graphics_tests shader-headers)
    add_test(NAME agc_buffer_alignment COMMAND agc_driver_graphics_tests buffer-alignment)
    add_test(NAME agc_pixel_inputs COMMAND agc_driver_graphics_tests pixel-inputs)
    add_test(NAME agc_depth_attachment_state COMMAND agc_driver_graphics_tests depth-target)
    add_test(NAME agc_depth_shader COMMAND agc_driver_graphics_tests depth-shader)
    if(WIN32)
        set_tests_properties(agc_depth_shader agc_depth_attachment_state agc_shader_stages agc_scan_conversion agc_clipping_state agc_provoking_vertex agc_color_control agc_shader_headers agc_buffer_alignment agc_pixel_inputs PROPERTIES ENVIRONMENT_MODIFICATION
                "PATH=path_list_prepend:$<TARGET_FILE_DIR:libc>")
    endif()
endif()

if(BUILD_TESTING)
    add_executable(agc_depth_layout_tests tests/DepthTargetLayout.cpp Graphics/src/DepthTargetLayout.cpp)
    target_include_directories(agc_depth_layout_tests PRIVATE ${LIBS_INCLUDE_DIR})
    target_link_libraries(agc_depth_layout_tests PRIVATE libc)
    set_target_properties(agc_depth_layout_tests PROPERTIES CXX_EXTENSIONS OFF)
    configure_windows_unwind(agc_depth_layout_tests)
    add_test(NAME agc_depth_layout COMMAND agc_depth_layout_tests)
    add_executable(agc_memory_type_tests tests/MemoryType.cpp)
    target_include_directories(agc_memory_type_tests PRIVATE ${LIBS_INCLUDE_DIR} ${CMAKE_SOURCE_DIR}/3rdparty/Vulkan-Headers/include)
    target_link_libraries(agc_memory_type_tests PRIVATE libc)
    set_target_properties(agc_memory_type_tests PROPERTIES CXX_EXTENSIONS OFF)
    configure_windows_unwind(agc_memory_type_tests)
    add_test(NAME agc_memory_type COMMAND agc_memory_type_tests)
    add_executable(agc_depth_state_tests tests/DepthState.cpp Graphics/src/DepthState.cpp Graphics/src/DepthTargetLayout.cpp)
    target_include_directories(agc_depth_state_tests PRIVATE ${LIBS_INCLUDE_DIR} ${CMAKE_SOURCE_DIR}/3rdparty/Vulkan-Headers/include)
    target_link_libraries(agc_depth_state_tests PRIVATE libc)
    set_target_properties(agc_depth_state_tests PROPERTIES CXX_EXTENSIONS OFF)
    configure_windows_unwind(agc_depth_state_tests)
    add_test(NAME agc_depth_state COMMAND agc_depth_state_tests)
    add_executable(agc_buffer_operands_tests tests/BufferOperands.cpp)
    target_link_libraries(agc_buffer_operands_tests PRIVATE shader_recompiler_agc_driver libc)
    set_target_properties(agc_buffer_operands_tests PROPERTIES CXX_EXTENSIONS OFF)
    configure_windows_unwind(agc_buffer_operands_tests)
    add_test(NAME agc_buffer_operands COMMAND agc_buffer_operands_tests)
    add_executable(agc_vertex_entry_tests tests/VertexEntry.cpp)
    target_link_libraries(agc_vertex_entry_tests PRIVATE shader_recompiler_agc_driver libc)
    set_target_properties(agc_vertex_entry_tests PROPERTIES CXX_EXTENSIONS OFF)
    configure_windows_unwind(agc_vertex_entry_tests)
    add_test(NAME agc_vertex_entry COMMAND agc_vertex_entry_tests)
    add_executable(agc_shader_clause_tests tests/ShaderClause.cpp)
    add_executable(agc_sampler_tests tests/Sampler.cpp tests/GuestSamplerResource.cpp
            Graphics/src/GuestSamplerResource.cpp Graphics/src/Sampler.cpp)
    target_include_directories(agc_sampler_tests PRIVATE ${LIBS_INCLUDE_DIR} ${CMAKE_SOURCE_DIR}/3rdparty/Vulkan-Headers/include)
    target_link_libraries(agc_sampler_tests PRIVATE libc)
    set_target_properties(agc_sampler_tests PROPERTIES CXX_EXTENSIONS OFF)
    configure_windows_unwind(agc_sampler_tests)
    add_test(NAME agc_sampler COMMAND agc_sampler_tests)
    add_executable(agc_shader_legacy_multiply_tests tests/ShaderLegacyMultiply.cpp)
    target_link_libraries(agc_shader_legacy_multiply_tests PRIVATE shader_recompiler_agc_driver libc)
    set_target_properties(agc_shader_legacy_multiply_tests PROPERTIES CXX_EXTENSIONS OFF)
    configure_windows_unwind(agc_shader_legacy_multiply_tests)
    add_test(NAME agc_shader_legacy_multiply COMMAND agc_shader_legacy_multiply_tests)
    add_executable(agc_shader_half_class_tests tests/ShaderHalfClass.cpp)
    target_link_libraries(agc_shader_half_class_tests PRIVATE shader_recompiler_agc_driver libc)
    set_target_properties(agc_shader_half_class_tests PROPERTIES CXX_EXTENSIONS OFF)
    configure_windows_unwind(agc_shader_half_class_tests)
    add_test(NAME agc_shader_half_class COMMAND agc_shader_half_class_tests)
    add_executable(agc_shader_integer_compare_tests tests/ShaderIntegerCompare.cpp)
    target_link_libraries(agc_shader_integer_compare_tests PRIVATE shader_recompiler_agc_driver libc)
    set_target_properties(agc_shader_integer_compare_tests PROPERTIES CXX_EXTENSIONS OFF)
    configure_windows_unwind(agc_shader_integer_compare_tests)
    add_test(NAME agc_shader_integer_compare COMMAND agc_shader_integer_compare_tests)
    target_link_libraries(agc_shader_clause_tests PRIVATE shader_recompiler_agc_driver libc)
    set_target_properties(agc_shader_clause_tests PROPERTIES CXX_EXTENSIONS OFF)
    configure_windows_unwind(agc_shader_clause_tests)
    add_test(NAME agc_shader_clause COMMAND agc_shader_clause_tests)
    if(WIN32)
        set_tests_properties(agc_memory_type agc_depth_state agc_depth_layout agc_buffer_operands agc_vertex_entry agc_shader_clause agc_shader_legacy_multiply agc_shader_half_class agc_shader_integer_compare agc_sampler PROPERTIES ENVIRONMENT_MODIFICATION
                "PATH=path_list_prepend:$<TARGET_FILE_DIR:libc>")
    endif()
endif()

add_executable(agc_driver_bda_device_tests EXCLUDE_FROM_ALL
        tests/BdaDevice.cpp
        tests/BdaShader.cpp
        tests/BdaExecution.cpp
        tests/BdaContracts.cpp
        Execution/src/BdaFeatures.cpp
        Graphics/src/Resources.cpp
        Graphics/src/BufferAddress.cpp
)

target_include_directories(agc_driver_bda_device_tests PRIVATE
        ${LIBS_INCLUDE_DIR}
        ${CMAKE_SOURCE_DIR}/core/shader/recompiler
        ${CMAKE_SOURCE_DIR}/3rdparty/Vulkan-Headers/include
)
target_link_libraries(agc_driver_bda_device_tests PRIVATE SDL2-static libc shader_recompiler_agc_driver)
set_target_properties(agc_driver_bda_device_tests PROPERTIES CXX_EXTENSIONS OFF)
configure_windows_unwind(agc_driver_bda_device_tests)

option(AGC_BUILD_VISUAL_TEST "Enable the standalone AGC SPIR-V visual test" OFF)

add_executable(agc_depth_device_tests EXCLUDE_FROM_ALL
        tests/DepthDevice.cpp Graphics/src/DepthSurface.cpp Graphics/src/DepthMemoryOwnership.cpp Graphics/src/DepthTargetLayout.cpp Execution/src/GuestMemory.cpp
        Graphics/src/Resources.cpp Graphics/src/BufferAddress.cpp Graphics/src/BufferPool.cpp)
target_include_directories(agc_depth_device_tests PRIVATE ${LIBS_INCLUDE_DIR}
        ${CMAKE_SOURCE_DIR}/core/shader/recompiler ${CMAKE_SOURCE_DIR}/3rdparty/Vulkan-Headers/include)
target_link_libraries(agc_depth_device_tests PRIVATE SDL2-static libc shader_recompiler_agc_driver)
set_target_properties(agc_depth_device_tests PROPERTIES CXX_EXTENSIONS OFF)
configure_windows_unwind(agc_depth_device_tests)
foreach(stage IN ITEMS vert frag)
    add_custom_command(OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/Depth.${stage}.spv"
            COMMAND "$<TARGET_FILE:glslang-standalone>" -V
            "${CMAKE_CURRENT_SOURCE_DIR}/tests/shaders/Depth.${stage}"
            -o "${CMAKE_CURRENT_BINARY_DIR}/Depth.${stage}.spv"
            DEPENDS glslang-standalone "${CMAKE_CURRENT_SOURCE_DIR}/tests/shaders/Depth.${stage}"
            VERBATIM)
    target_sources(agc_depth_device_tests PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/Depth.${stage}.spv")
endforeach()

if(AGC_BUILD_VISUAL_TEST)
    ensure_agc_glslang()

    set(visualShaders)

    foreach(stage IN ITEMS vert frag)
        set(shaderOutput "${CMAKE_CURRENT_BINARY_DIR}/Triangle.${stage}.spv")

        add_custom_command(
                OUTPUT "${shaderOutput}"
                COMMAND "$<TARGET_FILE:glslang-standalone>"
                -V
                --target-env vulkan1.1
                -o "${shaderOutput}"
                "${CMAKE_CURRENT_SOURCE_DIR}/tests/shaders/Triangle.${stage}"
                DEPENDS
                glslang-standalone
                "${CMAKE_CURRENT_SOURCE_DIR}/tests/shaders/Triangle.${stage}"
                VERBATIM
        )

        list(APPEND visualShaders "${shaderOutput}")
    endforeach()

    add_executable(agc_driver_visual_test EXCLUDE_FROM_ALL
            tests/Visual.cpp
            Execution/src/VulkanDevice.cpp
            Execution/src/SwapchainState.cpp
            Execution/src/AspectFit.cpp
            Execution/src/PresentationScaler.cpp
            Execution/src/GuestMemory.cpp
            Execution/src/BdaFeatures.cpp
            Graphics/src/Draw.cpp
            Graphics/src/DepthTargetLayout.cpp
            Graphics/src/DepthSurface.cpp
            Graphics/src/ColorTargetLayout.cpp
            Graphics/src/ColorTargetTransfer.cpp
            Graphics/src/Pipeline.cpp
            Graphics/src/Resources.cpp
            Graphics/src/ShaderResources.cpp
            Graphics/src/ShaderAddressResources.cpp
            Graphics/src/GuestBufferMemory.cpp
            Graphics/src/BdaResources.cpp
            Graphics/src/BufferAddress.cpp
            Graphics/src/ShaderValidation.cpp
            Graphics/src/GuestTextureResource.cpp
            Graphics/src/GuestSamplerResource.cpp
            Graphics/src/TextureFormat.cpp
            Graphics/src/TextureTiling.cpp
            Graphics/src/TextureDetiler.cpp
            Graphics/src/Texture.cpp
            Graphics/src/Sampler.cpp
            ${agcTextureDetileShaderHeader}
            ${visualShaders}
    )

    target_include_directories(agc_driver_visual_test PRIVATE
            ${LIBS_INCLUDE_DIR}
            ${agcTextureDetileGeneratedRoot}
            ${CMAKE_SOURCE_DIR}/core/shader/recompiler
            ${CMAKE_SOURCE_DIR}/core/shader/recompiler/IntermediateRepresentation/include
            ${CMAKE_SOURCE_DIR}/core/shader/recompiler/ControlFlow/include
            ${CMAKE_SOURCE_DIR}/core/shader/recompiler/RdnaDecoder/include
            ${CMAKE_SOURCE_DIR}/3rdparty/Vulkan-Headers/include
            ${CMAKE_SOURCE_DIR}/3rdparty/SPIRV-Headers/include
    )

    target_link_libraries(agc_driver_visual_test PRIVATE
            SDL2-static
            libc
    )

    set_target_properties(agc_driver_visual_test PROPERTIES CXX_EXTENSIONS OFF)
    configure_windows_unwind(agc_driver_visual_test)

    if(WIN32)
        set(visualRuntimeFiles "$<TARGET_FILE:libc>")

        if(MINGW)
            get_filename_component(visualCompilerDirectory "${CMAKE_CXX_COMPILER}" DIRECTORY)

            foreach(runtimeName IN ITEMS
                    libgcc_s_seh-1.dll
                    libstdc++-6.dll
                    libwinpthread-1.dll
            )
                set(runtimePath "${visualCompilerDirectory}/${runtimeName}")

                if(NOT EXISTS "${runtimePath}")
                    message(FATAL_ERROR "Cannot locate MinGW runtime: ${runtimeName}")
                endif()

                list(APPEND visualRuntimeFiles "${runtimePath}")
            endforeach()
        endif()

        add_custom_target(agc_driver_visual_runtime
                COMMAND "${CMAKE_COMMAND}" -E copy_if_different
                ${visualRuntimeFiles}
                "$<TARGET_FILE_DIR:agc_driver_visual_test>"
                DEPENDS libc
                COMMAND_EXPAND_LISTS
                VERBATIM
        )

        add_dependencies(agc_driver_visual_test agc_driver_visual_runtime)
    endif()
endif()

include(PostCompile.cmake)
