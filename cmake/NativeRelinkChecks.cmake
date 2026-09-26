if(BUILD_TESTING AND TARGET agc_sampler_tests)
    # The shared graphics fixture declarations include shader compiler types.
    target_include_directories(agc_sampler_tests PRIVATE ${CMAKE_SOURCE_DIR}/core/shader/recompiler)
endif()

if(AGC_NATIVE_RELINKED_ONLY)
    target_compile_definitions(libSceAgcDriver PUBLIC AGC_NATIVE_RELINKED_ONLY=1)
    if(UNIX AND NOT APPLE)
        foreach(nativeTarget IN ITEMS libSceAgcDriver libSceAgc libSceVideoOut)
            target_link_options(${nativeTarget} PRIVATE "LINKER:--no-undefined")
        endforeach()
    endif()

    if(BUILD_TESTING)
        add_executable(native_host_exception_tests tests/NativeHostExceptions.cpp)
        target_include_directories(native_host_exception_tests PRIVATE ${CMAKE_SOURCE_DIR}/core/libs)
        target_link_libraries(native_host_exception_tests PRIVATE libc)
        add_test(NAME native_host_exceptions COMMAND native_host_exception_tests)
        if(WIN32)
            set_tests_properties(native_host_exceptions PROPERTIES ENVIRONMENT_MODIFICATION
                "PATH=path_list_prepend:$<TARGET_FILE_DIR:libc>")
        endif()
        find_package(Python3 QUIET COMPONENTS Interpreter)
        if(Python3_Interpreter_FOUND AND UNIX AND NOT APPLE)
            add_test(NAME native_build_contract COMMAND ${Python3_EXECUTABLE}
                ${CMAKE_SOURCE_DIR}/tests/native_build_contract.py
                $<TARGET_FILE:libSceAgcDriver> $<TARGET_FILE:libSceAgc> $<TARGET_FILE:libSceVideoOut>)
        endif()
    endif()
endif()

if(BUILD_TESTING)
    add_executable(native_import_lowering_tests
        tests/NativeImportLowering.cpp
        core/relinker/relinker/src/analysis/ImportLibraries.cpp
        core/relinker/relinker/src/analysis/AgcImportLowering.cpp
        core/relinker/relinker/src/analysis/AgcLoweringAnalyzer.cpp
        core/relinker/relinker/src/analysis/CallSiteResolver.cpp
        core/relinker/codegen/src/x86/X64InstructionDecoder.cpp
        core/libs/nid/src/NidCompute.cpp core/libs/nid/src/Sha1.cpp)
    target_include_directories(native_import_lowering_tests PRIVATE
        core/relinker/domain/include core/relinker/relinker/include
        core/relinker/codegen/include core/libs/nid/include)
    add_test(NAME native_import_lowering COMMAND native_import_lowering_tests)
endif()

if(BUILD_TESTING AND Python3_Interpreter_FOUND AND (WIN32 OR (UNIX AND NOT APPLE)))
    add_test(NAME native_function_relink COMMAND ${Python3_EXECUTABLE}
        ${CMAKE_SOURCE_DIR}/tests/native_function_relink.py
        $<TARGET_FILE:relinker> ${CMAKE_CXX_COMPILER})
endif()

if(BUILD_TESTING AND AGC_NATIVE_RELINKED_ONLY)
    add_executable(native_failure_propagation_tests tests/NativeFailurePropagation.cpp)
    target_include_directories(native_failure_propagation_tests PRIVATE core/libs 3rdparty/Vulkan-Headers/include)
    target_link_libraries(native_failure_propagation_tests PRIVATE libSceAgc libSceAgcDriver libc)
    add_test(NAME native_failure_propagation COMMAND native_failure_propagation_tests)
    set_tests_properties(native_failure_propagation PROPERTIES TIMEOUT 10)
    if(WIN32)
        set_tests_properties(native_failure_propagation PROPERTIES ENVIRONMENT_MODIFICATION
            "PATH=path_list_prepend:$<TARGET_FILE_DIR:libSceAgcDriver>")
    endif()
endif()

if(BUILD_TESTING AND AGC_NATIVE_RELINKED_ONLY)
    add_executable(native_shader_argument_tests tests/NativeShaderArguments.cpp)
    target_include_directories(native_shader_argument_tests PRIVATE core/libs 3rdparty/Vulkan-Headers/include)
    target_link_libraries(native_shader_argument_tests PRIVATE libSceAgcDriver libc)
    add_test(NAME native_shader_arguments COMMAND native_shader_argument_tests)
    if(WIN32)
        set_tests_properties(native_shader_arguments PROPERTIES ENVIRONMENT_MODIFICATION
            "PATH=path_list_prepend:$<TARGET_FILE_DIR:libSceAgcDriver>")
    endif()
endif()

if(BUILD_TESTING AND AGC_NATIVE_RELINKED_ONLY AND MINGW)
    foreach(nativeTest IN ITEMS native_host_exception_tests native_failure_propagation_tests native_shader_argument_tests)
        target_compile_options(${nativeTest} PRIVATE -fno-asynchronous-unwind-tables)
        configure_windows_unwind(${nativeTest})
    endforeach()
endif()

if(BUILD_TESTING AND CMAKE_SYSTEM_PROCESSOR MATCHES "AMD64|amd64|x86_64" AND CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_executable(native_function_execution_tests
        tests/NativeFunctionExecution.cpp
        core/relinker/relinker/src/lowering/NativeFunctions.cpp
        core/relinker/codegen/src/x86/X64InstructionDecoder.cpp)
    target_include_directories(native_function_execution_tests PRIVATE
        core/relinker/domain/include core/relinker/relinker/include core/relinker/codegen/include)
    add_test(NAME native_function_execution COMMAND native_function_execution_tests)
endif()

if(BUILD_TESTING AND AGC_NATIVE_RELINKED_ONLY)
    add_executable(native_completion_gpu_tests EXCLUDE_FROM_ALL tests/NativeCompletionGpu.cpp)
    target_include_directories(native_completion_gpu_tests PRIVATE core/libs core/shader/recompiler
        3rdparty/Vulkan-Headers/include)
    target_link_libraries(native_completion_gpu_tests PRIVATE libSceAgcDriver libc)
    if(MINGW)
        target_compile_options(native_completion_gpu_tests PRIVATE -fno-asynchronous-unwind-tables)
        configure_windows_unwind(native_completion_gpu_tests)
    endif()
    foreach(stage IN ITEMS vert frag)
        set(completionShader "${CMAKE_CURRENT_BINARY_DIR}/NativeCompletion.${stage}.spv")
        add_custom_command(OUTPUT "${completionShader}"
            COMMAND "$<TARGET_FILE:glslang-standalone>" -V --target-env vulkan1.1
                "${CMAKE_SOURCE_DIR}/core/libs/prx/libSceAgcDriver/tests/shaders/Triangle.${stage}"
                -o "${completionShader}"
            DEPENDS glslang-standalone "${CMAKE_SOURCE_DIR}/core/libs/prx/libSceAgcDriver/tests/shaders/Triangle.${stage}"
            VERBATIM)
        target_sources(native_completion_gpu_tests PRIVATE "${completionShader}")
    endforeach()
endif()
