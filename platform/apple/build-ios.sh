cmake -S . -B build-ios -G Xcode \
         -D CMAKE_SYSTEM_NAME=iOS \
         -D CMAKE_OSX_DEPLOYMENT_TARGET=14.0 \
         -D CMAKE_OSX_ARCHITECTURES="arm64" \
         -D CMAKE_OSX_SYSROOT=$(xcrun --sdk iphoneos --show-sdk-path)