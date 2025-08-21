cmake_minimum_required(VERSION 3.16)

project(OpenGLCourseTemplate VERSION 1.0 LANGUAGES CXX C)

# OFF (default) = Build a single Host Application with all chapters.
# ON            = Build one specific chapter as a standalone executable.
option(BUILD_STANDALONE "Build each chapter as a separate executable" OFF)
if(BUILD_STANDALONE)
    set(STANDALONE_CHAPTER_NAME "Chapter10-TransformationsCube" CACHE STRING "The name of the single chapter to build when BUILD_STANDALONE is ON")
endif()
set(DEVELOPMENT_TEAM_ID "8MRFDR3542" CACHE STRING "Apple Developer Team ID for code signing")
set(BUNDLE_IDENTIFIER_PREFIX "com.adu.muh" CACHE STRING "Base bundle identifier for Apple targets")

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set the output directory for binaries
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(platforms) 
include(CPM)
set(CPM_SOURCE_CACHE ${CMAKE_CURRENT_SOURCE_DIR}/.cache/cpm)

if(PLATFORM_IS_ANDROID)
    option(CHAPTER_TO_BUILD_FOR_ANDROID "Chapter01-Window" "The chapter to build for Android.")
    set(BUILD_SHARED_LIBS OFF CACHE BOOL "Force static linking for all dependencies" FORCE)
    set(CMAKE_DEBUG_POSTFIX "" CACHE STRING "No postfix for debug builds" FORCE)
    set(CMAKE_POSITION_INDEPENDENT_CODE ON)
    message(STATUS "Android target chapter: ${CHAPTER_TO_BUILD_FOR_ANDROID}")

    if(WIN32)
        set(GRADLEW_COMMAND "${CMAKE_SOURCE_DIR}/platform/android/gradlew.bat")
    else()
        set(GRADLEW_COMMAND "${CMAKE_SOURCE_DIR}/platform/android/gradlew")
    endif()

    add_custom_target(runGradleBuildDebug
        COMMAND ${GRADLEW_COMMAND} -PchapterToBuild=${CHAPTER_TO_BUILD_FOR_ANDROID} installDebug
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/platform/android
        COMMENT "Running Gradle build for ${CHAPTER_TO_BUILD_FOR_ANDROID} in Debug mode..."
        VERBATIM
    )
    add_custom_target(runGradleBuildRelease
        COMMAND ${GRADLEW_COMMAND} -PchapterToBuild=${CHAPTER_TO_BUILD_FOR_ANDROID} installRelease
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/platform/android
        COMMENT "Running Gradle build for ${CHAPTER_TO_BUILD_FOR_ANDROID} in Release mode..."
        VERBATIM
    )

else()
    set(BUILD_SHARED_LIBS OFF CACHE BOOL "Force static linking for all dependencies" FORCE)
endif()

# SDL (Windowing and Input)
CPMAddPackage(
    NAME SDL3
    GITHUB_REPOSITORY libsdl-org/SDL
    GIT_TAG release-3.2.16
)
set_target_properties(SDL3_test PROPERTIES FOLDER "External Libraries/SDL3")
set_target_properties(SDL_uclibc PROPERTIES FOLDER "External Libraries/SDL3")
set_target_properties(SDL3-static PROPERTIES FOLDER "External Libraries/SDL3")


# GLM (Mathematics)
CPMAddPackage("gh:g-truc/glm#1.0.1")
set_target_properties(glm PROPERTIES FOLDER "External Libraries/glm")

# ImGui (Debug UI)
CPMAddPackage(
    NAME imgui
    GITHUB_REPOSITORY ocornut/imgui
    GIT_TAG v1.91.9b-docking
)

if(imgui_SOURCE_DIR) 
    set(ROOT ${imgui_SOURCE_DIR})
    message("IMGUI ROOT: ${imgui_SOURCE_DIR}")

    add_library(imgui STATIC
        ${ROOT}/imgui.cpp
        ${ROOT}/imgui_draw.cpp
        ${ROOT}/imgui_tables.cpp
        ${ROOT}/imgui_widgets.cpp
        ${ROOT}/imgui_demo.cpp
    )

    target_include_directories(imgui PUBLIC
        $<BUILD_INTERFACE:${ROOT}>
        $<INSTALL_INTERFACE:include>
    )

    set_property(TARGET imgui PROPERTY CXX_STANDARD ${CMAKE_CXX_STANDARD})
    target_compile_definitions(imgui PRIVATE IMGUI_IMPL_API=)
    set_target_properties(imgui PROPERTIES FOLDER "External Libraries/imgui")

    set(IMGUI_BACKEND_INSTALL_TARGETS "")
    set(IMGUI_BACKEND_INSTALL_HEADERS "")

    add_library(imgui_impl_sdl3 STATIC ${ROOT}/backends/imgui_impl_sdl3.cpp)

    target_link_libraries(imgui_impl_sdl3 PUBLIC imgui SDL3::SDL3)
    target_include_directories(imgui_impl_sdl3 PUBLIC
        $<BUILD_INTERFACE:${ROOT}>
        $<INSTALL_INTERFACE:include>
    )
    list(APPEND IMGUI_BACKEND_INSTALL_TARGETS imgui_impl_sdl3)
    list(APPEND IMGUI_BACKEND_INSTALL_HEADERS ${ROOT}/backends/imgui_impl_sdl3.h)
    target_compile_definitions(imgui_impl_sdl3 PRIVATE IMGUI_IMPL_API=)
    set_target_properties(imgui_impl_sdl3 PROPERTIES FOLDER "External Libraries/imgui")
    set_target_properties(imgui_impl_sdl3 PROPERTIES CXX_STANDARD ${CMAKE_CXX_STANDARD})

    add_library(imgui_impl_opengl3 STATIC ${ROOT}/backends/imgui_impl_opengl3.cpp)

    if(PLATFORM_IS_DESKTOP)
        find_package(OpenGL REQUIRED)
        target_link_libraries(imgui_impl_opengl3 PUBLIC imgui OpenGL::GL)
    elseif(PLATFORM_IS_ANDROID)
        target_link_libraries(imgui_impl_opengl3 PUBLIC imgui GLESv3)
    elseif(PLATFORM_IS_IOS)
        target_link_libraries(imgui_impl_opengl3 PUBLIC imgui "-framework OpenGLES")
    elseif(PLATFORM_IS_EMSCRIPTEN)
        target_link_libraries(imgui_impl_opengl3 PUBLIC imgui GLESv2)
    endif()
    
    target_include_directories(imgui_impl_opengl3 PUBLIC
        $<BUILD_INTERFACE:${ROOT}>
        $<INSTALL_INTERFACE:include>
    )
    set_property(TARGET imgui_impl_opengl3 PROPERTY CXX_STANDARD ${CMAKE_CXX_STANDARD})
    target_compile_definitions(imgui_impl_opengl3 PRIVATE IMGUI_IMPL_API=)
    list(APPEND IMGUI_BACKEND_INSTALL_TARGETS imgui_impl_opengl3)
    list(APPEND IMGUI_BACKEND_INSTALL_HEADERS ${ROOT}/backends/imgui_impl_opengl3.h)
    set_target_properties(imgui_impl_opengl3 PROPERTIES FOLDER "External Libraries/imgui")
endif()


# GLAD (OpenGL Function Loader)
# can do: pip install glad
# glad --generator=c --spec=gl --api="gl=4.6" --profile=core --out-path=glad_output
CPMAddPackage(
    NAME glad
    GITHUB_REPOSITORY Dav1dde/glad
    VERSION "2.0.8"
)
add_subdirectory("${glad_SOURCE_DIR}/cmake" glad_cmake)
if(PLATFORM_IS_ANDROID)
    # For Android
    glad_add_library(glad_android
        REPRODUCIBLE 
        API egl:version=1.5 gles2:version=3.2)
    set_target_properties(glad_android PROPERTIES FOLDER "External Libraries/glad")
elseif(PLATFORM_IS_DESKTOP)
    glad_add_library(glad
        REPRODUCIBLE  
        API             gl:core=4.6
        EXTENSIONS      GL_ARB_debug_output 
                        GL_ARB_texture_storage
                        GL_ARB_bindless_texture) 
    set_target_properties(glad PROPERTIES FOLDER "External Libraries/glad")
elseif(PLATFORM_IS_EMSCRIPTEN)
    # For Emscripten / WebGL 2.0
    glad_add_library(   glad 
        REPRODUCIBLE 
        DEBUG
        API             gles2:version=3.0
        EXTENSIONS      GL_KHR_debug
    )
    set_target_properties(glad PROPERTIES FOLDER "External Libraries/glad")
elseif(PLATFORM_IS_IOS)
    # For iOS
    glad_add_library(glad REPRODUCIBLE API gles2:version=3.0)
    set_target_properties(glad PROPERTIES FOLDER "External Libraries/glad")
endif()

# STB (Image Loading)
CPMAddPackage(
    NAME stb
    GITHUB_REPOSITORY nothings/stb
    GIT_TAG master
)

# spdlog (Logging)
set(BUILD_TESTING OFF CACHE BOOL "A regular cached option")
if(EMSCRIPTEN)
    # Pretend Threads are found to avoid CMake error in spdlog and others
    set(CMAKE_THREAD_LIBS_INIT "-lpthread")
    set(CMAKE_HAVE_THREADS_LIBRARY 1)
    set(CMAKE_USE_WIN32_THREADS_INIT 0)
    set(CMAKE_USE_PTHREADS_INIT 1)
    set(THREADS_PREFER_PTHREAD_FLAG ON)
    find_package(Python3 COMPONENTS Interpreter QUIET)
endif()

CPMAddPackage(
    NAME spdlog
    GITHUB_REPOSITORY gabime/spdlog
    GIT_TAG v1.15.3
    OPTIONS
        "SPDLOG_BUILD_TESTING OFF"
)
if(TARGET spdlog_headers_for_ide)
    set_target_properties(spdlog_headers_for_ide PROPERTIES FOLDER "External Libraries/spdlog")
endif()
if(TARGET spdlog-utests)
    set_target_properties(spdlog-utests PROPERTIES FOLDER "External Libraries/spdlog")
endif()
set_target_properties(spdlog PROPERTIES FOLDER "External Libraries/spdlog")


# Tracy (Profiling frames)
CPMAddPackage(
    NAME tracy
    GITHUB_REPOSITORY wolfpld/tracy
    GIT_TAG v0.11.1
    OPTIONS
        "TRACY_ENABLE ON"
)

if(TARGET TracyClient)
    set_target_properties(TracyClient PROPERTIES FOLDER "External Libraries/tracy")
endif()
# Tracy also creates other targets like 'Tracy' (the server) and 'Capture'
# You can group them too if they are built.
if(TARGET Tracy)
    set_target_properties(Tracy PROPERTIES FOLDER "External Libraries/tracy/Tools")
endif()
if(TARGET Capture)
    set_target_properties(Capture PROPERTIES FOLDER "External Libraries/tracy/Tools")
endif()

# Assimp (Model Loading)
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "Disable Assimp's unit tests" FORCE)

set(ASSIMP_BUILD_ALL_IMPORTERS_BY_DEFAULT OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ALL_EXPORTERS_BY_DEFAULT OFF CACHE BOOL "" FORCE)

set(ASSIMP_BUILD_OBJ_IMPORTER ON CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_GLTF_IMPORTER ON CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_FBX_IMPORTER ON CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_STL_IMPORTER ON CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_COLLADA_IMPORTER ON CACHE BOOL "" FORCE)
set(ASSIMP_INSTALL OFF CACHE BOOL "" FORCE)
set(ASSIMP_WARNINGS_AS_ERRORS OFF CACHE BOOL "" FORCE)
SET(CPACK_COMPONENTS_ALL assimp_bin ${LIBASSIMP_COMPONENT} ${LIBASSIMP_DEV_COMPONENT} assimp_dev)


CPMAddPackage(
    NAME assimp
    GITHUB_REPOSITORY assimp/assimp
    VERSION 5.2.5
)
if(TARGET assimp AND CMAKE_COMPILER_IS_GNUCXX)
    target_compile_options(assimp PRIVATE "-Wno-error=dangling-reference")
endif()
set_target_properties(assimp PROPERTIES FOLDER "External Libraries/assimp")
# These targets may or may not be generated depending on the version and platform
if(TARGET unit)
    set_target_properties(unit PROPERTIES FOLDER "External Libraries/assimp/tests")
endif()
if(TARGET UpdateAssimpLibsDebugSymbolsAndDLLs)
    set_target_properties(UpdateAssimpLibsDebugSymbolsAndDLLs PROPERTIES FOLDER "External Libraries/assimp/utils")
endif()
if(TARGET zlibstatic)
    set_target_properties(zlibstatic PROPERTIES FOLDER "External Libraries/assimp/utils")
endif()

file(GLOB_RECURSE ASSET_FILES CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/assets/*"
)
add_custom_target(CopyAssets ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different
            "${CMAKE_SOURCE_DIR}/assets"
            "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/assets"
    DEPENDS ${ASSET_FILES}
    COMMENT "Copying modified assets to build directory..."
)
# We also want folders to work for this target
set_target_properties(CopyAssets PROPERTIES FOLDER "Utility")

add_subdirectory(base)
add_subdirectory(chapters)

if(PLATFORM_IS_ANDROID)
    message(STATUS "Build Mode: Android (Chapter: ${CHAPTER_TO_BUILD_FOR_ANDROID})")
    if(IS_DIRECTORY "${CMAKE_SOURCE_DIR}/chapters/${CHAPTER_TO_BUILD_FOR_ANDROID}")
        add_chapter(${CHAPTER_TO_BUILD_FOR_ANDROID})
    else()
        message(FATAL_ERROR "Chapter directory not found: ${CHAPTER_TO_BUILD_FOR_ANDROID}")
    endif()

elseif(BUILD_STANDALONE)
    if(STANDALONE_CHAPTER_NAME)
        # Build one specific standalone chapter
        message(STATUS "Build Mode: Standalone (Single Chapter: ${STANDALONE_CHAPTER_NAME})")
        if(IS_DIRECTORY "${CMAKE_SOURCE_DIR}/chapters/${STANDALONE_CHAPTER_NAME}")
            add_chapter(${STANDALONE_CHAPTER_NAME})
        else()
            message(FATAL_ERROR "Chapter directory not found: ${STANDALONE_CHAPTER_NAME}")
        endif()
    else()
        # Build all available standalone chapters
        message(STATUS "Build Mode: Standalone (All Chapters)")
        file(GLOB CHAPTER_DIRS LIST_DIRECTORIES true "${CMAKE_SOURCE_DIR}/chapters/*")
        foreach(DIR ${CHAPTER_DIRS})
            get_filename_component(CHAPTER_NAME ${DIR} NAME)
            if(EXISTS "${DIR}/main.cpp")
                message(STATUS "Found standalone-ready chapter: ${CHAPTER_NAME}")
                add_chapter(${CHAPTER_NAME})
            endif()
        endforeach()
    endif()

else() # Bundled Host Application Mode (Default for Desktop/Web)
    message(STATUS "Build Mode: Bundled Host Application")

    # Glob sources for all chapters
    file(GLOB_RECURSE ALL_CHAPTER_SOURCES CONFIGURE_DEPENDS
        "${CMAKE_SOURCE_DIR}/chapters/*/Chapter*.cpp"
    )

    add_executable(OpenGLCourseApp
        bundledApp/main.cpp
        bundledApp/BundledApp.cpp
        ${ALL_CHAPTER_SOURCES}
    )
    target_link_libraries(OpenGLCourseApp PRIVATE base)
    add_dependencies(OpenGLCourseApp CopyAssets)

    target_include_directories(OpenGLCourseApp PRIVATE
        "bundledApp"
        "chapters"
        "base"
    )
    set_target_properties(OpenGLCourseApp PROPERTIES FOLDER "Course Projects")

    if(PLATFORM_IS_EMSCRIPTEN)
        set(ASSETS_SOURCE_PATH "${CMAKE_SOURCE_DIR}/assets")
        set_target_properties(OpenGLCourseApp PROPERTIES SUFFIX ".html")
        target_link_options(OpenGLCourseApp PRIVATE
            "SHELL:-s FORCE_FILESYSTEM=1"
            "SHELL:-s FULL_ES3"
            "SHELL:-s MIN_WEBGL_VERSION=2"
            "SHELL:-s ALLOW_MEMORY_GROWTH=1"
            "SHELL:-s USE_SDL=3"
            "SHELL:--use-preload-plugins"
            "SHELL:--preload-file ${ASSETS_SOURCE_PATH}@/assets"
        )
    endif()
endif()

set(CPACK_COMPONENTS_ALL Applications Development)
include(CPack)
if(NOT DEFINED CPACK_PACKAGE_NAME)
  set(CPACK_PACKAGE_NAME "${PROJECT_NAME}")
endif()
if(NOT DEFINED CPACK_PACKAGE_VERSION)
  set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
endif()