#include <unordered_map>
#include "InputConfig.hpp"
#include "DOM.hpp"
#include "InputModifier.hpp"
#include "KeyCode.hpp"
#include "LogLocal.hpp"
#include "StringUtil.hpp"
#include "TomlLoader.hpp"

namespace{
    using RawActionMappings = std::unordered_map<Smol::ActionName, Smol::KeyBindings>;

    Smol::ActionMappings convert(const RawActionMappings& raw){
        using namespace Smol;

        ActionMappings mappings;

        for(const auto& [name, bindings]: raw){
            mappings.emplace_back(ActionInfo{
                .name = name,
                .mappings = bindings,
                .count = 0
            });
        }

        return mappings;
    }

    Smol::KeyCode toKeyCode(Smol::StrView str){
        using namespace Smol;
        using enum KeyCode;

        static StringHashMap<KeyCode> map = {
            {"Num0", Num0},
            {"Num1", Num1},
            {"Num2", Num2},
            {"Num3", Num3},
            {"Num4", Num4},
            {"Num5", Num5},
            {"Num6", Num6},
            {"Num7", Num7},
            {"Num8", Num8},
            {"Num9", Num9},
            {"A", A},
            {"B", B},
            {"C", C},
            {"D", D},
            {"E", E},
            {"F", F},
            {"G", G},
            {"H", H},
            {"I", I},
            {"K", K},
            {"L", L},
            {"M", M},
            {"N", N},
            {"O", O},
            {"P", P},
            {"Q", Q},
            {"R", R},
            {"S", S},
            {"T", T},
            {"U", U},
            {"V", V},
            {"W", W},
            {"X", X},
            {"Y", Y},
            {"Z", Z},
            {"F1", F1},
            {"F2", F2},
            {"F3", F3},
            {"F4", F4},
            {"F5", F5},
            {"F6", F6},
            {"F7", F7},
            {"F8", F8},
            {"F9", F9},
            {"F10", F10},
            {"F11", F11},
            {"F12", F12},
            {"Ctrl", Ctrl},
            {"Alt", Alt},
            {"Shift", Shift},
            {"Tab", Tab},
            {"Space", Space},
            {"Enter", Enter},
            {"Escape", Escape}
        };

        auto upper = toUpper(str);
        if(auto it = map.find(upper); it != map.end()){
            return it->second;
        }

        LOG_ERROR("Unknown SwizzleOrder: {}", str);
        return Unknown;
    }

    std::vector<Smol::InputModifier> gatherModifiers(const Smol::DOM::Value& node){
        using namespace Smol;

        std::vector<InputModifier> modifiers;

        node.forEach("modifiers", [&modifiers](const DOM::Value& node){
            modifiers.emplace_back(CreateInputModifier(node));
        });

        return modifiers;
    }
}

namespace Smol
{
    InputConfig TomlTraits<InputConfig>::from(
        const DOM::Value& dom,
        const TomlMetadata& metadata
    ){
        SMOL_ASSERT(metadata.type == "input");

        RawActionMappings raw;

        dom.forEach("context.mappings", [&raw](const DOM::Value& node){
            auto key = node.get<Str>("key");
            auto action = node.get<Str>("action");

            if(!key || !action) return;

            raw[*action].emplace_back(
                KeyBinding{
                    .keyCode = toKeyCode(*key),
                    .modifiers = gatherModifiers(node)
                }
            );
        });

        return InputConfig{
            .mappings = ::convert(raw)
        };
    }
}
