add_library(libSceAgcDriver SHARED EXCLUDE_FROM_ALL
        ${CMAKE_CURRENT_SOURCE_DIR}/Execution/src/DisplayBuffer.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Execution/src/Driver.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Execution/src/GuestMemory.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Execution/src/Pm4.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Execution/src/VulkanDevice.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Graphics/src/Draw.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Graphics/src/Pipeline.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Graphics/src/Resources.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Graphics/src/ShaderInputState.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Graphics/src/ShaderResources.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Graphics/src/ShaderValidation.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Graphics/src/State.cpp
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

target_include_directories(libSceAgcDriver PRIVATE ${LIBS_INCLUDE_DIR})
target_include_directories(libSceAgcDriver PRIVATE ${CMAKE_SOURCE_DIR}/3rdparty/Vulkan-Headers/include)
target_include_directories(libSceAgcDriver PRIVATE ${CMAKE_SOURCE_DIR}/3rdparty/SPIRV-Headers/include)
add_shader_recompiler(shader_recompiler_agc_driver libc)
target_link_libraries(libSceAgcDriver PRIVATE shader_recompiler_agc_driver SDL2-static libc)
set_target_properties(libSceAgcDriver PROPERTIES CXX_EXTENSIONS OFF)
configure_windows_unwind(libSceAgcDriver)

foreach(runtimeLinkFlag IN ITEMS -static-libgcc -static-libstdc++ -static)
    string(REPLACE "${runtimeLinkFlag}" "" CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS}")
endforeach()
add_executable(agc_driver_tests EXCLUDE_FROM_ALL tests/Submit.cpp)
target_include_directories(agc_driver_tests PRIVATE ${LIBS_INCLUDE_DIR})
target_link_libraries(agc_driver_tests PRIVATE libSceAgcDriver libc)
set_target_properties(agc_driver_tests PROPERTIES CXX_EXTENSIONS OFF)
configure_windows_unwind(agc_driver_tests)

add_executable(agc_driver_recompiler_tests EXCLUDE_FROM_ALL tests/Recompiler.cpp)
target_include_directories(agc_driver_recompiler_tests PRIVATE ${LIBS_INCLUDE_DIR})
target_link_libraries(agc_driver_recompiler_tests PRIVATE libSceAgcDriver libc)
set_target_properties(agc_driver_recompiler_tests PROPERTIES CXX_EXTENSIONS OFF)
configure_windows_unwind(agc_driver_recompiler_tests)

add_executable(agc_driver_flip_tests EXCLUDE_FROM_ALL tests/Flip.cpp)
target_include_directories(agc_driver_flip_tests PRIVATE ${LIBS_INCLUDE_DIR})
target_link_libraries(agc_driver_flip_tests PRIVATE libSceAgcDriver libc)
set_target_properties(agc_driver_flip_tests PROPERTIES CXX_EXTENSIONS OFF)
configure_windows_unwind(agc_driver_flip_tests)

add_executable(agc_driver_pm4_tests EXCLUDE_FROM_ALL tests/Pm4.cpp)
target_include_directories(agc_driver_pm4_tests PRIVATE ${LIBS_INCLUDE_DIR})
target_link_libraries(agc_driver_pm4_tests PRIVATE libSceAgcDriver libc)
set_target_properties(agc_driver_pm4_tests PROPERTIES CXX_EXTENSIONS OFF)
configure_windows_unwind(agc_driver_pm4_tests)

add_executable(agc_driver_graphics_tests EXCLUDE_FROM_ALL
        tests/Graphics.cpp
        Graphics/src/State.cpp
        Graphics/src/Resources.cpp
        Graphics/src/ShaderResources.cpp
        Execution/src/GuestMemory.cpp
)
target_include_directories(agc_driver_graphics_tests PRIVATE ${LIBS_INCLUDE_DIR} ${CMAKE_SOURCE_DIR}/core/shader/recompiler ${CMAKE_SOURCE_DIR}/3rdparty/Vulkan-Headers/include)
target_link_libraries(agc_driver_graphics_tests PRIVATE libc)
set_target_properties(agc_driver_graphics_tests PROPERTIES CXX_EXTENSIONS OFF)
configure_windows_unwind(agc_driver_graphics_tests)

option(AGC_BUILD_VISUAL_TEST "Enable the standalone AGC SPIR-V visual test" OFF)
if(AGC_BUILD_VISUAL_TEST)
    function(add_agc_test_glslang)
        set(BUILD_SHARED_LIBS OFF)
        set(BUILD_EXTERNAL OFF)
        set(GLSLANG_TESTS OFF)
        set(GLSLANG_ENABLE_INSTALL OFF)
        set(ENABLE_GLSLANG_BINARIES ON)
        set(ENABLE_SPIRV ON)
        set(ENABLE_SPVREMAPPER OFF)
        set(ENABLE_OPT OFF)
        add_subdirectory("${CMAKE_SOURCE_DIR}/3rdparty/glslang" "${CMAKE_BINARY_DIR}/3rdparty/glslang" EXCLUDE_FROM_ALL)
    endfunction()
    add_agc_test_glslang()
    set(visualShaders)
    foreach(stage IN ITEMS vert frag)
        set(shaderOutput "${CMAKE_CURRENT_BINARY_DIR}/Triangle.${stage}.spv")
        add_custom_command(OUTPUT "${shaderOutput}"
                COMMAND "$<TARGET_FILE:glslang-standalone>" -V --target-env vulkan1.1 -o "${shaderOutput}" "${CMAKE_CURRENT_SOURCE_DIR}/tests/shaders/Triangle.${stage}"
                DEPENDS glslang-standalone "${CMAKE_CURRENT_SOURCE_DIR}/tests/shaders/Triangle.${stage}"
                VERBATIM)
        list(APPEND visualShaders "${shaderOutput}")
    endforeach()
    add_executable(agc_driver_visual_test EXCLUDE_FROM_ALL
            tests/Visual.cpp
            Execution/src/VulkanDevice.cpp
            Execution/src/GuestMemory.cpp
            Graphics/src/Draw.cpp
            Graphics/src/Pipeline.cpp
            Graphics/src/Resources.cpp
            Graphics/src/ShaderResources.cpp
            Graphics/src/ShaderValidation.cpp
            ${visualShaders})
    target_include_directories(agc_driver_visual_test PRIVATE ${LIBS_INCLUDE_DIR} ${CMAKE_SOURCE_DIR}/core/shader/recompiler ${CMAKE_SOURCE_DIR}/3rdparty/Vulkan-Headers/include ${CMAKE_SOURCE_DIR}/3rdparty/SPIRV-Headers/include)
    target_link_libraries(agc_driver_visual_test PRIVATE SDL2-static libc)
    set_target_properties(agc_driver_visual_test PROPERTIES CXX_EXTENSIONS OFF)
    configure_windows_unwind(agc_driver_visual_test)
    if(WIN32)
        set(visualRuntimeFiles "$<TARGET_FILE:libc>")
        if(MINGW)
            get_filename_component(visualCompilerDirectory "${CMAKE_CXX_COMPILER}" DIRECTORY)
            foreach(runtimeName IN ITEMS libgcc_s_seh-1.dll libstdc++-6.dll libwinpthread-1.dll)
                set(runtimePath "${visualCompilerDirectory}/${runtimeName}")
                if(NOT EXISTS "${runtimePath}")
                    message(FATAL_ERROR "Cannot locate MinGW runtime: ${runtimeName}")
                endif()
                list(APPEND visualRuntimeFiles "${runtimePath}")
            endforeach()
        endif()
        add_custom_target(agc_driver_visual_runtime
                COMMAND "${CMAKE_COMMAND}" -E copy_if_different ${visualRuntimeFiles} "$<TARGET_FILE_DIR:agc_driver_visual_test>"
                DEPENDS libc
                COMMAND_EXPAND_LISTS
                VERBATIM)
        add_dependencies(agc_driver_visual_test agc_driver_visual_runtime)
    endif()
endif()
