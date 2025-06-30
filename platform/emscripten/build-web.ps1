clear
emsdk_env.ps1
cmake -S . -B web-build -G Ninja -DCMAKE_TOOLCHAIN_FILE=$env:EMSDK\upstream\emscripten\cmake\Modules\Platform\Emscripten.cmake
cmake --build .\web-build\
