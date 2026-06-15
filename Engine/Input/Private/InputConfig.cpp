#include <unordered_map>
#include "DOM.hpp"
#include "EnumUtil.hpp"
#include "InputConfig.hpp"
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
    template<>
    CStr EnumTraits<KeyCode>::name = "KeyCode";

    template<>
    KeyCode EnumTraits<KeyCode>::convert(StrView str){
        using enum KeyCode;

        static StringHashMap<KeyCode> map = {
            {"NUM0", Num0},
            {"NUM1", Num1},
            {"NUM2", Num2},
            {"NUM3", Num3},
            {"NUM4", Num4},
            {"NUM5", Num5},
            {"NUM6", Num6},
            {"NUM7", Num7},
            {"NUM8", Num8},
            {"NUM9", Num9},
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
            {"CTRL", Ctrl},
            {"ALT", Alt},
            {"SHIFT", Shift},
            {"TAB", Tab},
            {"SPACE", Space},
            {"ENTER", Enter},
            {"ESCAPE", Escape}
        };

        auto upper = toUpper(str);
        if(auto it = map.find(upper); it != map.end()){
            return it->second;
        }

        return Unknown;
    }

    template<>
    CStr EnumTraits<MouseButton>::name = "MouseButton";

    template<>
    MouseButton EnumTraits<MouseButton>::convert(StrView str){
        using enum MouseButton;

        static StringHashMap<MouseButton> map = {
            {"LEFTMOUSEBUTTON", LButton},
            {"RIGHTMOUSEBUTTON", RButton}
        };

        auto upper = toUpper(str);
        if(auto it = map.find(upper); it != map.end()){
            return it->second;
        }

        return Unknown;
    }

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

            auto keyCode = EnumTraits<KeyCode>::convert(*key);
            if(keyCode != KeyCode::Unknown){
                raw[*action].emplace_back(
                    KeyBinding{
                        .cond = keyCode,
                        .modifiers = gatherModifiers(node)
                    }
                );

                return;
            }

            auto mouseButton = EnumTraits<MouseButton>::convert(*key);
            if(mouseButton != MouseButton::Unknown){
                raw[*action].emplace_back(
                    KeyBinding{
                        .cond = mouseButton,
                        .modifiers = gatherModifiers(node)
                    }
                );

                return;
            }

            LOG_ERROR("Unknown KeyCode: {}", *key);
        });

        return InputConfig{
            .mappings = ::convert(raw)
        };
    }
}
