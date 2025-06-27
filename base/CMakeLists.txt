# GLOB_RECURSE if subdirs created
file(GLOB SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
file(GLOB HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/*.hpp)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} PREFIX "Source Files" FILES ${SOURCES})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} PREFIX "Header Files" FILES ${HEADERS})

if(ANDROID)
    add_library(base STATIC ${SOURCES} ${HEADERS})
else()
    add_library(base STATIC ${SOURCES} ${HEADERS})
endif()

set_target_properties(base PROPERTIES
    FOLDER "Course Projects/Base Library"
)

target_include_directories(base PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${stb_SOURCE_DIR}
    ${tracy_SOURCE_DIR}
)

target_link_libraries(base PUBLIC platform_definitions)

target_link_libraries(base PUBLIC
    glm
    SDL3::SDL3
    imgui
    imgui_impl_sdl3
    imgui_impl_opengl3
    spdlog
    assimp
)

if(ANDROID)
    target_link_libraries(base PUBLIC glad_android log android)
else()
    target_link_libraries(base PUBLIC glad)
endif()

# Enable Tracy profiling if not on Emscripten
if (NOT CMAKE_SYSTEM_NAME STREQUAL "Emscripten")
    target_compile_definitions(base PRIVATE TRACY_ENABLE)
    target_link_libraries(base PRIVATE Tracy::TracyClient)
endif()
