#include <gtest/gtest.h>
#include "DOM.hpp"
#include "InputModifier.hpp"
#include "TomlLoader.hpp"

using namespace Smol;

TEST(InputModifierTest, NegateModifier){
    NegateModifier modifier(true, true, true);

    InputValue value(1, 2, 3);
    auto modified = Apply(modifier, value);

    auto vec = modified.GetAxis3D();
    EXPECT_FLOAT_EQ(vec.x, -1.0f);
    EXPECT_FLOAT_EQ(vec.y, -2.0f);
    EXPECT_FLOAT_EQ(vec.z, -3.0f);
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

    auto vec = modified.GetAxis3D();
    EXPECT_FLOAT_EQ(vec.x, -1.0f);
    EXPECT_FLOAT_EQ(vec.y, -2.0f);
    EXPECT_FLOAT_EQ(vec.z, -3.0f);
}

TEST(InputModifierTest, ScaleModifierSingleFactor){
    ScaleModifier modifier(2);

    InputValue value(1, 2, 3);
    auto modified = Apply(modifier, value);

    auto vec = modified.GetAxis3D();
    EXPECT_FLOAT_EQ(vec.x, 2.0f);
    EXPECT_FLOAT_EQ(vec.y, 4.0f);
    EXPECT_FLOAT_EQ(vec.z, 6.0f);
}

TEST(InputModifierTest, ScaleModifierMultipleFactor){
    ScaleModifier modifier(0.5f, 2.0f, 3.0f);

    InputValue value(1, 2, 3);
    auto modified = Apply(modifier, value);

    auto vec = modified.GetAxis3D();
    EXPECT_FLOAT_EQ(vec.x, 0.5f);
    EXPECT_FLOAT_EQ(vec.y, 4.0f);
    EXPECT_FLOAT_EQ(vec.z, 9.0f);
}

TEST(InputModifierTest, ScaleModifierSingleFactorFromTOML){
    constexpr auto TOML_TEXT = R"(
        [modifier]
        type = "Scale"
        factor = 3
    )";
    auto dom = parseTomlString(TOML_TEXT);
    auto mod = dom.at("modifier");
    ASSERT_TRUE(mod != nullptr);

    auto modifier = CreateInputModifier(*mod);
    ASSERT_TRUE(std::holds_alternative<ScaleModifier>(modifier));

    InputValue value(1, 2, 3);
    auto modified = Apply(modifier, value);

    auto vec = modified.GetAxis3D();
    EXPECT_FLOAT_EQ(vec.x, 3.0f);
    EXPECT_FLOAT_EQ(vec.y, 6.0f);
    EXPECT_FLOAT_EQ(vec.z, 9.0f);
}

TEST(InputModifierTest, ScaleModifierMultipleFactorFromTOML){
    constexpr auto TOML_TEXT = R"(
        [modifier]
        type = "Scale"
        factor = [2, 3, 0.5]
    )";
    auto dom = parseTomlString(TOML_TEXT);
    auto mod = dom.at("modifier");
    ASSERT_TRUE(mod != nullptr);

    auto modifier = CreateInputModifier(*mod);
    ASSERT_TRUE(std::holds_alternative<ScaleModifier>(modifier));

    InputValue value(1, 2, 3);
    auto modified = Apply(modifier, value);

    auto vec = modified.GetAxis3D();
    EXPECT_FLOAT_EQ(vec.x, 2.0f);
    EXPECT_FLOAT_EQ(vec.y, 6.0f);
    EXPECT_FLOAT_EQ(vec.z, 1.5f);
}

TEST(InputModifierTest, SwizzleModifier){
    SwizzleModifier modifier(SwizzleOrder::YXZ);

    InputValue value(1, 2, 3);
    auto modified = Apply(modifier, value);

    auto vec = modified.GetAxis3D();
    EXPECT_FLOAT_EQ(vec.x, 2.0f);
    EXPECT_FLOAT_EQ(vec.y, 1.0f);
    EXPECT_FLOAT_EQ(vec.z, 3.0f);
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

    auto vec = modified.GetAxis3D();
    EXPECT_FLOAT_EQ(vec.x, 2.0f);
    EXPECT_FLOAT_EQ(vec.y, 1.0f);
    EXPECT_FLOAT_EQ(vec.z, 3.0f);
}
