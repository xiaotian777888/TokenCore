
mkdir ios
cd ios
#x86_64 for for simulator
cmake ..  -G'Unix Makefiles' -DCMAKE_TOOLCHAIN_FILE=../tools/ios-cmake/ios.toolchain.cmake -DIOS_DEPLOYMENT_TARGET=9.0 -DCMAKE_OSX_ARCHITECTURES=arm64 -DCMAKE_CONFIGURATION_TYPES=Release
make