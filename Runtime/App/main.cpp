#include "AppConfig.hpp"
#include "AppMainLoop.hpp"
#include "OS.hpp"

using namespace Smol;

constexpr CStr APP_CONFIG_PATH = "Config/DefaultApp.toml";

int main(int, char*[]){
    auto config = loadTomlFile<AppConfig>(APP_CONFIG_PATH);

    OS os(config.runtime);
    // auto device = Smol::createDevice();
    AppMainLoop mainLoop(config);

    os.Run(mainLoop);

    return 0;
}