#include <gtest/gtest.h>
#include "DOM.hpp"
#include "InputModifier.hpp"
#include "TomlLoader.hpp"

using namespace Smol;

TEST(InputModifierTest, SwizzleModifier){
    SwizzleModifier modifier(SwizzleOrder::YXZ);

    InputValue value(1, 2, 3);
    auto modified = Apply(modifier, value);

    auto vec = modified.Get<Vec3>();
    EXPECT_EQ(vec.x, 2.0f);
    EXPECT_EQ(vec.y, 1.0f);
    EXPECT_EQ(vec.z, 3.0f);
}

TEST(InputModifierTest, SwizzleModifierFromTOML){
    constexpr auto TOML_TEXT = R"(
        [modifier]
        type = "Swizzle"
        order = "YXZ"
    )";
    auto dom = parseTomlString(TOML_TEXT);
    auto mod = dom.at("modifier");
    ASSERT_TRUE(mod != nullptr);

    auto modifier = CreateInputModifier(*mod);
    ASSERT_TRUE(std::holds_alternative<SwizzleModifier>(modifier));

    InputValue value(1, 2, 3);
    auto modified = Apply(modifier, value);

    auto vec = modified.Get<Vec3>();
    EXPECT_EQ(vec.x, 2.0f);
    EXPECT_EQ(vec.y, 1.0f);
    EXPECT_EQ(vec.z, 3.0f);
}

TEST(InputModifierTest, NegateModifier){
    NegateModifier modifier(true, true, true);

    InputValue value(1, 2, 3);
    auto modified = Apply(modifier, value);

    auto vec = modified.Get<Vec3>();
    EXPECT_EQ(vec.x, -1.0f);
    EXPECT_EQ(vec.y, -2.0f);
    EXPECT_EQ(vec.z, -3.0f);
}

TEST(InputModifierTest, NegateModifierFromTOML){
    constexpr auto TOML_TEXT = R"(
        [modifier]
        type = "Negate"
    )";
    auto dom = parseTomlString(TOML_TEXT);
    auto mod = dom.at("modifier");
    ASSERT_TRUE(mod != nullptr);

    auto modifier = CreateInputModifier(*mod);
    ASSERT_TRUE(std::holds_alternative<NegateModifier>(modifier));

    InputValue value(1, 2, 3);
    auto modified = Apply(modifier, value);

    auto vec = modified.Get<Vec3>();
    EXPECT_EQ(vec.x, -1.0f);
    EXPECT_EQ(vec.y, -2.0f);
    EXPECT_EQ(vec.z, -3.0f);
}
