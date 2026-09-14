add_library(libSceAgcDriver SHARED EXCLUDE_FROM_ALL
        ${CMAKE_CURRENT_SOURCE_DIR}/Execution/src/Driver.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Execution/src/VulkanDevice.cpp
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
