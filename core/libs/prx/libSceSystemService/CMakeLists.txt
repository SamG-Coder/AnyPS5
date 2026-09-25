add_library(libSceSystemService SHARED EXCLUDE_FROM_ALL
    ${CMAKE_CURRENT_SOURCE_DIR}/Export.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Browser.cpp
)
target_include_directories(libSceSystemService PRIVATE ${LIBS_INCLUDE_DIR})
target_link_libraries(libSceSystemService PRIVATE SDL2-static)
configure_windows_unwind(libSceSystemService)
