#include "TomlLoader.hpp"

namespace Smol
{
    TomlMetadata TomlTraits<TomlMetadata>::from(
        const DOM::Value& root
    ){
        auto version = root.get<u32>("metadata.version")
            .value_or(0);
        auto name = root.get<Str>("metadata.name")
            .value_or("UnnamedToml");

        return TomlMetadata{
            .version = version,
            .name = name
        };
    }
}
