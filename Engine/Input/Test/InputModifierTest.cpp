#include <gtest/gtest.h>
#include "DOM.hpp"
#include "ModifierRegistry.hpp"
#include "StringUtil.hpp"
#include "TomlLoader.hpp"

const auto TEST_TOML = R"(
[[mappings]]
key = "W"
action = "Move"
modifiers = [
    {type = "Swizzle", order = "YXZ"}
]
[[mappings]]
key = "A"
action = "Move"
modifiers = [
    {type = "Negate"}
]
[[mappings]]
key = "S"
action = "Move"
modifiers = [
    {type = "Swizzle", order = "YXZ"},
    {type = "Negate"}
]
[[mappings]]
key = "D"
action = "Move"
)";

using namespace Smol;

class InputModifierTest: public ::testing::Test{
private:
    static DOM::Value dom;
    static StringHashMap<const DOM::Array*> mappings;

protected:
    static ModifierRegistry registry;

    static void SetUpTestSuite(){
        dom = parseTomlString(TEST_TOML);

        dom.forEach("mappings", [&](const DOM::Value& node){
            auto key = node.get<Str>("key");
            auto modifiers = node.at("modifiers");
            if(!key.has_value() || modifiers == nullptr)
                return;

            mappings.try_emplace(key.value(), modifiers->asArray());
        });
    }

    static const DOM::Value* FindMapping(StrView key, usize index){
        if(auto it = mappings.find(key); it != mappings.end()){
            auto& arr = (*it->second);
            return &arr[index];
        }

        return nullptr;
    }
};

DOM::Value InputModifierTest::dom;
StringHashMap<const DOM::Array*> InputModifierTest::mappings;
ModifierRegistry InputModifierTest::registry;

TEST_F(InputModifierTest, SwizzleModifier){
    auto modifierConfig = FindMapping("W", 0);
    ASSERT_TRUE(modifierConfig != nullptr);
    auto modifier = registry.Create(*modifierConfig);
    ASSERT_TRUE(modifier != nullptr);

    InputValue value{
        .raw = Vec3(1, 2, 3)
    };
    auto modified = (*modifier)(value);

    EXPECT_EQ(modified.raw.x, 2.0f);
    EXPECT_EQ(modified.raw.y, 1.0f);
    EXPECT_EQ(modified.raw.z, 3.0f);
}

TEST_F(InputModifierTest, NegateModifier){
    auto modifierConfig = FindMapping("A", 0);
    ASSERT_TRUE(modifierConfig != nullptr);
    auto modifier = registry.Create(*modifierConfig);
    ASSERT_TRUE(modifier != nullptr);

    InputValue value{
        .raw = Vec3(1, 2, 3)
    };
    auto modified = (*modifier)(value);

    EXPECT_EQ(modified.raw.x, -1.0f);
    EXPECT_EQ(modified.raw.y, -2.0f);
    EXPECT_EQ(modified.raw.z, -3.0f);
}
