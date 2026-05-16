#include "AppConfig.hpp"
#include "OS.hpp"

namespace Smol
{
    AppConfig TomlTraits<AppConfig>::from(const DOM::Value& root){
        AppConfig config{
            .runtime = TomlTraits<RuntimeConfig>::from(root)
        };

        return config;
    }

}