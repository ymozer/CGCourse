Emscripten
https://stunlock.gg/posts/emscripten_with_cmake/



git clone https://github.com/emscripten-core/emsdk

git pull
emsdk list
emsdk install latest
emsdk activate --permanent latest
emsdk\emsdk_env.bat or source ./emsdk_env.sh

# From your project root, create a separate build directory
cmake -S . -B build-web -D CMAKE_TOOLCHAIN_FILE=%{EMSDK_PATH}/upstream/emscripten/cmake/Modules/Platform/Emscripten.cmake -DCMAKE_BUILD_TYPE=Release 

or emcmake