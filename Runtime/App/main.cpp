#include "AppConfig.hpp"
#include "AppMainLoop.hpp"
#include "OS.hpp"
#include "RHIDevice.hpp"

using namespace Smol;

constexpr CStr APP_CONFIG_PATH = "Config/DefaultApp.toml";

int main(int, char*[]){
    auto config = loadTomlFile<AppConfig>(APP_CONFIG_PATH);

    auto device = Smol::CreateDevice();
    OS os(config.runtime, *device);
    AppMainLoop mainLoop(config, *device);

    os.Run(mainLoop, *device);

    return 0;
}
