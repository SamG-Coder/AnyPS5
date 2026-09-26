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

if(BUILD_TESTING AND Python3_Interpreter_FOUND AND UNIX AND NOT APPLE)
    add_test(NAME native_function_relink COMMAND ${Python3_EXECUTABLE}
        ${CMAKE_SOURCE_DIR}/tests/native_function_relink.py
        $<TARGET_FILE:relinker> ${CMAKE_CXX_COMPILER})
endif()
