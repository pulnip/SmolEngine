#include <exception>
#include "AppConfig.hpp"
#include "AppMainLoop.hpp"
#include "OS.hpp"
#include "RHIDevice.hpp"
#include "RHITexture.hpp"
#include "Log.hpp"

using namespace Smol;

constexpr CStr APP_CONFIG_PATH = "Config/Default.app.toml";

i32 main(i32, CStr[]){
#if defined(_DEBUG) || !defined(NDEBUG)
    SetLogLevel(LogLevel::Warn);
#else
    SetLogLevel(LogLevel::Error);
#endif
    auto config = loadTomlFile<AppConfig>(APP_CONFIG_PATH);

    try{
        auto device = Smol::CreateDevice();

        OS os(config.runtime, *device);
        AppMainLoop mainLoop(config, os, *device);

        os.Run(mainLoop, *device);
    }
    catch(const std::exception& e){
        LOG_FATAL("Main",
            "Exception: {}",
            e.what()
        );

        return 1;
    }

    return 0;
}
