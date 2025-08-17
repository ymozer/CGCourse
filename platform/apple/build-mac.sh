cmake .. -G Xcode \
         -D CMAKE_SYSTEM_NAME=iOS \
         -D CMAKE_OSX_DEPLOYMENT_TARGET=14.0 \
         -D CMAKE_OSX_ARCHITECTURES="arm64" 