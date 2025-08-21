# Install script for directory: /Users/metin/CGCourse/.cache/cpm/assimp/2675/code

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/Users/metin/Libraries/emsdk/upstream/emscripten/cache/sysroot")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Debug")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "TRUE")
endif()

# Set path to fallback-tool for dependency-resolution.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "/usr/bin/objdump")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "libassimp5.2.4-dev" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "/Users/metin/CGCourse/web-build/lib/libassimpd.a")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "assimp-dev" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/assimp" TYPE FILE FILES
    "/Users/metin/CGCourse/.cache/cpm/assimp/2675/code/../include/assimp/anim.h"
    "/Users/metin/CGCourse/.cache/cpm/assimp/2675/code/../include/assimp/aabb.h"
    "/Users/metin/CGCourse/.cache/cpm/assimp/2675/code/../include/assimp/ai_assert.h"
    "/Users/metin/CGCourse/.cache/cpm/assimp/2675/code/../include/assimp/camera.h"
    "/Users/metin/CGCourse/.cache/cpm/assimp/2675/code/../include/assimp/color4.h"
    "/Users/metin/CGCourse/.cache/cpm/assimp/2675/code/../include/assimp/color4.inl"
    "/Users/metin/CGCourse/web-build/_deps/assimp-build/code/../include/assimp/config.h"
    "/Users/metin/CGCourse/.cache/cpm/assimp/2675/code/../include/assimp/ColladaMetaData.h"
    "/Users/metin/CGCourse/.cache/cpm/assimp/2675/code/../include/assimp/commonMetaData.h"
    "/Users/metin/CGCourse/.cache/cpm/assimp/2675/code/../include/assimp/defs.h"
    "/Users/metin/CGCourse/.cache/cpm/assimp/2675/code/../include/assimp/cfileio.h"
    "/Users/metin/CGCourse/.cache/cpm/assimp/2675/code/../include/assimp/light.h"
    "/Users/metin/CGCourse/.cache/cpm/assimp/2675/code/../include/assimp/material.h"
    "/Users/metin/CGCourse/.cache/cpm/assimp/2675/code/../include/assimp/material.inl"
    "/Users/metin/CGCourse/.cache/cpm/assimp/2675/code/../include/assimp/matrix3x3.h"
    "/Users/metin/CGCourse/.cache/cpm/assimp/2675/code/../include/assimp/matrix3x3.inl"
    "/Users/metin/CGCourse/.cache/cpm/assimp/2675/code/../include/assimp/matrix4x4.h"
    "/Users/metin/CGCourse/.cache/cpm/assimp/2675/code/../include/assimp/matrix4x4.inl"
    "/Users/metin/CGCourse/.cache/cpm/assimp/2675/code/../include/assimp/mesh.h"
    "/Users/metin/CGCourse/.cache/cpm/assimp/2675/code/../include/assimp/ObjMaterial.h"
    "/Users/metin/CGCourse/.cache/cpm/assimp/2675/code/../include/assimp/pbrmaterial.h"
    "/Users/metin/CGCourse/.cache/cpm/assimp/2675/code/../include/assimp/GltfMaterial.h"
    "/Users/metin/CGCourse/.cache/cpm/assimp/2675/code/../include/assimp/postprocess.h"
    "/Users/metin/CGCourse/.cache/cpm/assimp/2675/code/../include/assimp/quaternion.h"
    "/Users/metin/CGCourse/.cache/cpm/assimp/2675/code/../include/assimp/quaternion.inl"
    "/Users/metin/CGCourse/.cache/cpm/assimp/2675/code/../include/assimp/scene.h"
    "/Users/metin/CGCourse/.cache/cpm/assimp/2675/code/../include/assimp/metadata.h"
    "/Users/metin/CGCourse/.cache/cpm/assimp/2675/code/../include/assimp/texture.h"
    "/Users/metin/CGCourse/.cache/cpm/assimp/2675/code/../include/assimp/types.h"
    "/Users/metin/CGCourse/.cache/cpm/assimp/2675/code/../include/assimp/vector2.h"
    "/Users/metin/CGCourse/.cache/cpm/assimp/2675/code/../include/assimp/vector2.inl"
    "/Users/metin/CGCourse/.cache/cpm/assimp/2675/code/../include/assimp/vector3.h"
    "/Users/metin/CGCourse/.cache/cpm/assimp/2675/code/../include/assimp/vector3.inl"
    "/Users/metin/CGCourse/.cache/cpm/assimp/2675/code/../include/assimp/version.h"
    "/Users/metin/CGCourse/.cache/cpm/assimp/2675/code/../include/assimp/cimport.h"
    "/Users/metin/CGCourse/.cache/cpm/assimp/2675/code/../include/assimp/importerdesc.h"
    "/Users/metin/CGCourse/.cache/cpm/assimp/2675/code/../include/assimp/Importer.hpp"
    "/Users/metin/CGCourse/.cache/cpm/assimp/2675/code/../include/assimp/DefaultLogger.hpp"
    "/Users/metin/CGCourse/.cache/cpm/assimp/2675/code/../include/assimp/ProgressHandler.hpp"
    "/Users/metin/CGCourse/.cache/cpm/assimp/2675/code/../include/assimp/IOStream.hpp"
    "/Users/metin/CGCourse/.cache/cpm/assimp/2675/code/../include/assimp/IOSystem.hpp"
    "/Users/metin/CGCourse/.cache/cpm/assimp/2675/code/../include/assimp/Logger.hpp"
    "/Users/metin/CGCourse/.cache/cpm/assimp/2675/code/../include/assimp/LogStream.hpp"
    "/Users/metin/CGCourse/.cache/cpm/assimp/2675/code/../include/assimp/NullLogger.hpp"
    "/Users/metin/CGCourse/.cache/cpm/assimp/2675/code/../include/assimp/cexport.h"
    "/Users/metin/CGCourse/.cache/cpm/assimp/2675/code/../include/assimp/Exporter.hpp"
    "/Users/metin/CGCourse/.cache/cpm/assimp/2675/code/../include/assimp/DefaultIOStream.h"
    "/Users/metin/CGCourse/.cache/cpm/assimp/2675/code/../include/assimp/DefaultIOSystem.h"
    "/Users/metin/CGCourse/.cache/cpm/assimp/2675/code/../include/assimp/ZipArchiveIOSystem.h"
    "/Users/metin/CGCourse/.cache/cpm/assimp/2675/code/../include/assimp/SceneCombiner.h"
    "/Users/metin/CGCourse/.cache/cpm/assimp/2675/code/../include/assimp/fast_atof.h"
    "/Users/metin/CGCourse/.cache/cpm/assimp/2675/code/../include/assimp/qnan.h"
    "/Users/metin/CGCourse/.cache/cpm/assimp/2675/code/../include/assimp/BaseImporter.h"
    "/Users/metin/CGCourse/.cache/cpm/assimp/2675/code/../include/assimp/Hash.h"
    "/Users/metin/CGCourse/.cache/cpm/assimp/2675/code/../include/assimp/MemoryIOWrapper.h"
    "/Users/metin/CGCourse/.cache/cpm/assimp/2675/code/../include/assimp/ParsingUtils.h"
    "/Users/metin/CGCourse/.cache/cpm/assimp/2675/code/../include/assimp/StreamReader.h"
    "/Users/metin/CGCourse/.cache/cpm/assimp/2675/code/../include/assimp/StreamWriter.h"
    "/Users/metin/CGCourse/.cache/cpm/assimp/2675/code/../include/assimp/StringComparison.h"
    "/Users/metin/CGCourse/.cache/cpm/assimp/2675/code/../include/assimp/StringUtils.h"
    "/Users/metin/CGCourse/.cache/cpm/assimp/2675/code/../include/assimp/SGSpatialSort.h"
    "/Users/metin/CGCourse/.cache/cpm/assimp/2675/code/../include/assimp/GenericProperty.h"
    "/Users/metin/CGCourse/.cache/cpm/assimp/2675/code/../include/assimp/SpatialSort.h"
    "/Users/metin/CGCourse/.cache/cpm/assimp/2675/code/../include/assimp/SkeletonMeshBuilder.h"
    "/Users/metin/CGCourse/.cache/cpm/assimp/2675/code/../include/assimp/SmallVector.h"
    "/Users/metin/CGCourse/.cache/cpm/assimp/2675/code/../include/assimp/SmoothingGroups.h"
    "/Users/metin/CGCourse/.cache/cpm/assimp/2675/code/../include/assimp/SmoothingGroups.inl"
    "/Users/metin/CGCourse/.cache/cpm/assimp/2675/code/../include/assimp/StandardShapes.h"
    "/Users/metin/CGCourse/.cache/cpm/assimp/2675/code/../include/assimp/RemoveComments.h"
    "/Users/metin/CGCourse/.cache/cpm/assimp/2675/code/../include/assimp/Subdivision.h"
    "/Users/metin/CGCourse/.cache/cpm/assimp/2675/code/../include/assimp/Vertex.h"
    "/Users/metin/CGCourse/.cache/cpm/assimp/2675/code/../include/assimp/LineSplitter.h"
    "/Users/metin/CGCourse/.cache/cpm/assimp/2675/code/../include/assimp/TinyFormatter.h"
    "/Users/metin/CGCourse/.cache/cpm/assimp/2675/code/../include/assimp/Profiler.h"
    "/Users/metin/CGCourse/.cache/cpm/assimp/2675/code/../include/assimp/LogAux.h"
    "/Users/metin/CGCourse/.cache/cpm/assimp/2675/code/../include/assimp/Bitmap.h"
    "/Users/metin/CGCourse/.cache/cpm/assimp/2675/code/../include/assimp/XMLTools.h"
    "/Users/metin/CGCourse/.cache/cpm/assimp/2675/code/../include/assimp/IOStreamBuffer.h"
    "/Users/metin/CGCourse/.cache/cpm/assimp/2675/code/../include/assimp/CreateAnimMesh.h"
    "/Users/metin/CGCourse/.cache/cpm/assimp/2675/code/../include/assimp/XmlParser.h"
    "/Users/metin/CGCourse/.cache/cpm/assimp/2675/code/../include/assimp/BlobIOSystem.h"
    "/Users/metin/CGCourse/.cache/cpm/assimp/2675/code/../include/assimp/MathFunctions.h"
    "/Users/metin/CGCourse/.cache/cpm/assimp/2675/code/../include/assimp/Exceptional.h"
    "/Users/metin/CGCourse/.cache/cpm/assimp/2675/code/../include/assimp/ByteSwapper.h"
    "/Users/metin/CGCourse/.cache/cpm/assimp/2675/code/../include/assimp/Base64.hpp"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "assimp-dev" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/assimp/Compiler" TYPE FILE FILES
    "/Users/metin/CGCourse/.cache/cpm/assimp/2675/code/../include/assimp/Compiler/pushpack1.h"
    "/Users/metin/CGCourse/.cache/cpm/assimp/2675/code/../include/assimp/Compiler/poppack1.h"
    "/Users/metin/CGCourse/.cache/cpm/assimp/2675/code/../include/assimp/Compiler/pstdint.h"
    )
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
if(CMAKE_INSTALL_LOCAL_ONLY)
  file(WRITE "/Users/metin/CGCourse/web-build/_deps/assimp-build/code/install_local_manifest.txt"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
endif()
