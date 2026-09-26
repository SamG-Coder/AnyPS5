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
