add_executable(nid_patcher
        ${CMAKE_CURRENT_SOURCE_DIR}/nid/src/Sha1.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/nid/src/NidCompute.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/nid/src/NidResolver.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/nid/src/ExportExclusions.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/nid/src/ElfNidPatcher.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/nid/src/PeNidPatcher.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/nid/src/BinaryPatcherFactory.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/nid/src/main.cpp
)
target_include_directories(nid_patcher PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/nid/include)
target_compile_features(nid_patcher PRIVATE cxx_std_20)

if(MINGW)
    add_compile_options(-fno-asynchronous-unwind-tables)
endif()

function(configure_windows_unwind target)
    if(MINGW)
        if(NOT EXISTS "${CMAKE_OBJCOPY}")
            message(FATAL_ERROR "Windows DWARF unwinding requires objcopy")
        endif()
        add_custom_command(TARGET ${target} POST_BUILD
                COMMAND "${CMAKE_OBJCOPY}" --rename-section .eh_frame=.ehfram "$<TARGET_FILE:${target}>"
                VERBATIM
        )
    endif()
endfunction()

set(LIBC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/prx/libc)
add_library(libc SHARED EXCLUDE_FROM_ALL)
set(LIBS_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
add_subdirectory(${LIBC_DIR} ${CMAKE_CURRENT_BINARY_DIR}/prx/libc)
target_include_directories(libc PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
configure_windows_unwind(libc)

set(UNPATCHED_DIR ${CMAKE_CURRENT_BINARY_DIR}/libs/unpatched)
set(PATCHED_DIR ${CMAKE_CURRENT_BINARY_DIR}/libs)

set_target_properties(libc PROPERTIES
        PREFIX ""
        SUFFIX .prx
        RUNTIME_OUTPUT_DIRECTORY ${UNPATCHED_DIR}
        LIBRARY_OUTPUT_DIRECTORY ${UNPATCHED_DIR}
)

set(LIBC_UNPATCHED ${UNPATCHED_DIR}/libc.prx)
set(LIBC_PATCHED ${PATCHED_DIR}/libc.prx)

add_custom_command(
        OUTPUT ${LIBC_PATCHED}
        COMMAND ${CMAKE_COMMAND} -E copy ${LIBC_UNPATCHED} ${LIBC_PATCHED}
        COMMAND $<TARGET_FILE:nid_patcher> libc ${LIBC_PATCHED}
        DEPENDS nid_patcher libc ${LIBC_UNPATCHED}
        COMMENT "Patching NIDs for libc"
)

file(GLOB LIBS_DIRS LIST_DIRECTORIES true ${CMAKE_CURRENT_SOURCE_DIR}/prx/*)
list(REMOVE_ITEM LIBS_DIRS ${LIBC_DIR})

set(PATCHED_OUTPUTS ${LIBC_PATCHED})

foreach(LIB_DIR ${LIBS_DIRS})
    if(IS_DIRECTORY ${LIB_DIR})
        get_filename_component(LIB_NAME ${LIB_DIR} NAME)

        if(EXISTS ${LIB_DIR}/CMakeLists.txt)
            set(LIBS_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
            add_subdirectory(${LIB_DIR} ${CMAKE_CURRENT_BINARY_DIR}/prx/${LIB_NAME})
        else()
            add_library(${LIB_NAME} SHARED EXCLUDE_FROM_ALL ${LIB_DIR}/Export.cpp)
            target_include_directories(${LIB_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
            configure_windows_unwind(${LIB_NAME})
        endif()

        if(LIB_NAME STREQUAL "libkernel")
            target_link_libraries(${LIB_NAME} PRIVATE libc)
        else()
            target_link_libraries(${LIB_NAME} PRIVATE libkernel libc)
        endif()

        set_target_properties(${LIB_NAME} PROPERTIES
                PREFIX ""
                SUFFIX .prx
                RUNTIME_OUTPUT_DIRECTORY ${UNPATCHED_DIR}
                LIBRARY_OUTPUT_DIRECTORY ${UNPATCHED_DIR}
        )

        set(UNPATCHED_FILE ${UNPATCHED_DIR}/${LIB_NAME}.prx)
        set(PATCHED_FILE ${PATCHED_DIR}/${LIB_NAME}.prx)

        add_custom_command(
                OUTPUT ${PATCHED_FILE}
                COMMAND ${CMAKE_COMMAND} -E copy ${UNPATCHED_FILE} ${PATCHED_FILE}
                COMMAND $<TARGET_FILE:nid_patcher> ${LIB_NAME} --preserve-exports "${LIBC_UNPATCHED}" "${PATCHED_FILE}"
                DEPENDS nid_patcher ${LIB_NAME} ${UNPATCHED_FILE} ${LIBC_UNPATCHED}
                COMMENT "Patching NIDs for ${LIB_NAME}"
        )

        list(APPEND PATCHED_OUTPUTS ${PATCHED_FILE})
    endif()
endforeach()

add_custom_target(libs DEPENDS ${PATCHED_OUTPUTS})

if(BUILD_TESTING)
    add_executable(guest_standard_stream_tests tests/GuestStandardStreams.cpp)
    target_include_directories(guest_standard_stream_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
    target_link_libraries(guest_standard_stream_tests PRIVATE libc)
    configure_windows_unwind(guest_standard_stream_tests)
    add_test(NAME guest_standard_streams COMMAND guest_standard_stream_tests)
    if(WIN32)
        set_tests_properties(guest_standard_streams PROPERTIES ENVIRONMENT_MODIFICATION
            "PATH=path_list_prepend:$<TARGET_FILE_DIR:libc>")
    endif()
    add_executable(guest_signal_tests tests/GuestSignals.cpp)
    target_include_directories(guest_signal_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
    target_link_libraries(guest_signal_tests PRIVATE libkernel libc)
    configure_windows_unwind(guest_signal_tests)
    add_test(NAME guest_signals COMMAND guest_signal_tests)
    if(WIN32)
        set_tests_properties(guest_signals PROPERTIES ENVIRONMENT_MODIFICATION
            "PATH=path_list_prepend:$<TARGET_FILE_DIR:libkernel>")
    endif()
    add_executable(guest_system_configuration_tests tests/GuestSystemConfiguration.cpp)
    target_include_directories(guest_system_configuration_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
    target_link_libraries(guest_system_configuration_tests PRIVATE libkernel libc)
    configure_windows_unwind(guest_system_configuration_tests)
    add_test(NAME guest_system_configuration COMMAND guest_system_configuration_tests)
    if(WIN32)
        set_tests_properties(guest_system_configuration PROPERTIES ENVIRONMENT_MODIFICATION
            "PATH=path_list_prepend:$<TARGET_FILE_DIR:libkernel>")
    endif()
    add_executable(guest_udp_tests tests/GuestUdp.cpp)
    target_include_directories(guest_udp_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
    target_link_libraries(guest_udp_tests PRIVATE libkernel libc)
    configure_windows_unwind(guest_udp_tests)
    add_test(NAME guest_udp COMMAND guest_udp_tests)
    set_tests_properties(guest_udp PROPERTIES TIMEOUT 20)
    if(WIN32)
        set_tests_properties(guest_udp PROPERTIES ENVIRONMENT_MODIFICATION
            "PATH=path_list_prepend:$<TARGET_FILE_DIR:libkernel>")
    endif()
    add_executable(guest_address_tests tests/GuestAddressConversion.cpp)
    target_include_directories(guest_address_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
    target_link_libraries(guest_address_tests PRIVATE libkernel libc)
    configure_windows_unwind(guest_address_tests)
    add_test(NAME guest_addresses COMMAND guest_address_tests)
    if(WIN32)
        set_tests_properties(guest_addresses PROPERTIES ENVIRONMENT_MODIFICATION
            "PATH=path_list_prepend:$<TARGET_FILE_DIR:libkernel>")
    endif()
    add_executable(guest_error_tests tests/GuestErrors.cpp)
    target_include_directories(guest_error_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
    target_link_libraries(guest_error_tests PRIVATE libc)
    configure_windows_unwind(guest_error_tests)
    add_test(NAME guest_errors COMMAND guest_error_tests)
    if(WIN32)
        set_tests_properties(guest_errors PROPERTIES ENVIRONMENT_MODIFICATION
            "PATH=path_list_prepend:$<TARGET_FILE_DIR:libc>")
    endif()
    add_executable(guest_resolver_tests tests/GuestResolver.cpp)
    target_include_directories(guest_resolver_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
    target_link_libraries(guest_resolver_tests PRIVATE libScePosixForWebKit)
    add_test(NAME guest_resolver COMMAND guest_resolver_tests)
    if(WIN32)
        set_tests_properties(guest_resolver PROPERTIES ENVIRONMENT_MODIFICATION
            "PATH=path_list_prepend:$<TARGET_FILE_DIR:libScePosixForWebKit>")
    endif()
    add_executable(mspace_tests tests/Mspace.cpp)
    target_include_directories(mspace_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
    target_link_libraries(mspace_tests PRIVATE libc)
    configure_windows_unwind(mspace_tests)
    add_test(NAME mspace COMMAND mspace_tests)
    if(WIN32)
        set_tests_properties(mspace PROPERTIES ENVIRONMENT_MODIFICATION
            "PATH=path_list_prepend:$<TARGET_FILE_DIR:libc>")
    endif()
    add_executable(guest_memory_tests tests/GuestMemory.cpp)
    target_include_directories(guest_memory_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
    target_link_libraries(guest_memory_tests PRIVATE libkernel libc)
    configure_windows_unwind(guest_memory_tests)
    add_test(NAME guest_memory COMMAND guest_memory_tests)
    if(WIN32)
        set_tests_properties(guest_memory PROPERTIES ENVIRONMENT_MODIFICATION
            "PATH=path_list_prepend:$<TARGET_FILE_DIR:libkernel>")
    endif()
    add_executable(guest_string_tests
        tests/GuestStrings.cpp
        prx/libc/src/StringExtensions.cpp
        prx/libScePosixForWebKit/Export.cpp)
    target_include_directories(guest_string_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
    add_test(NAME guest_strings COMMAND guest_string_tests)
endif()

add_executable(application_heap_tests EXCLUDE_FROM_ALL tests/ApplicationHeap.cpp prx/libc/src/ApplicationHeap.cpp)
target_include_directories(application_heap_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
set_target_properties(application_heap_tests PROPERTIES CXX_EXTENSIONS OFF)

add_executable(guest_locale_tests EXCLUDE_FROM_ALL tests/GuestLocale.cpp)
target_include_directories(guest_locale_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
set_target_properties(guest_locale_tests PROPERTIES CXX_EXTENSIONS OFF)
target_link_libraries(guest_locale_tests PRIVATE libc)
configure_windows_unwind(guest_locale_tests)

if(MINGW)
    foreach(runtimeLinkFlag IN ITEMS -static-libgcc -static-libstdc++ -static)
        string(REPLACE "${runtimeLinkFlag}" "" CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS}")
    endforeach()
    add_executable(windows_exception_tests EXCLUDE_FROM_ALL tests/WindowsExceptions.cpp)
    set_target_properties(windows_exception_tests PROPERTIES CXX_EXTENSIONS OFF)
    target_link_libraries(windows_exception_tests PRIVATE libc)
    configure_windows_unwind(windows_exception_tests)
endif()
