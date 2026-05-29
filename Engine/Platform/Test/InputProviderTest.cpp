#include <memory>
#include <gtest/gtest.h>
#include "CoreFWD.hpp"
#include "KeyCode.hpp"
#include "MockInputProvider.hpp"

using namespace Smol;

class InputProviderTest: public ::testing::Test{
protected:
    static RAII<MockInputProvider> provider;

    static void SetUpTestSuite(){
        provider = std::make_unique<MockInputProvider>();
    }

    void SetUp() override{
        provider->Reset();
    }

    bool IsKeyDown(KeyCode keyCode){
        return provider->IsKeyDown(keyCode);
    }
    bool IsKeyNone(KeyCode keyCode){
        auto keyState = provider->GetKeyState(keyCode);
        return keyState == KeyState::None;
    }
    bool IsKeyPressed(KeyCode keyCode){
        auto keyState = provider->GetKeyState(keyCode);
        return keyState == KeyState::Pressed;
    }
    bool IsKeyReleased(KeyCode keyCode){
        auto keyState = provider->GetKeyState(keyCode);
        return keyState == KeyState::Released;
    }
    bool IsKeyHeld(KeyCode keyCode){
        auto keyState = provider->GetKeyState(keyCode);
        return keyState == KeyState::Held;
    }
};

RAII<MockInputProvider> InputProviderTest::provider = nullptr;

TEST_F(InputProviderTest, KeyStateLifecycleOccurFirst){
    {
        SCOPED_TRACE("Frame 0");
        provider->Poll();

        // input occured, but applied at next frame.
        provider->PressKey(KeyCode::A);

        EXPECT_FALSE(IsKeyDown(KeyCode::A));
        EXPECT_TRUE (IsKeyNone(KeyCode::A));
        EXPECT_FALSE(IsKeyPressed(KeyCode::A));
        EXPECT_FALSE(IsKeyReleased(KeyCode::A));
        EXPECT_FALSE(IsKeyHeld(KeyCode::A));
    }

    {
        SCOPED_TRACE("Frame 1");
        provider->Poll();

        EXPECT_TRUE(IsKeyDown(KeyCode::A));
        EXPECT_FALSE(IsKeyNone(KeyCode::A));
        EXPECT_TRUE(IsKeyPressed(KeyCode::A));
        EXPECT_FALSE(IsKeyReleased(KeyCode::A));
        EXPECT_FALSE(IsKeyHeld(KeyCode::A));
    }

    {
        SCOPED_TRACE("Frame 2");
        provider->Poll();

        provider->ReleaseKey(KeyCode::A);

        EXPECT_TRUE(IsKeyDown(KeyCode::A));
        EXPECT_FALSE(IsKeyNone(KeyCode::A));
        EXPECT_FALSE(IsKeyPressed(KeyCode::A));
        EXPECT_FALSE(IsKeyReleased(KeyCode::A));
        EXPECT_TRUE(IsKeyHeld(KeyCode::A));
    }

    {
        SCOPED_TRACE("Frame 3");
        provider->Poll();

        EXPECT_FALSE(IsKeyDown(KeyCode::A));
        EXPECT_FALSE(IsKeyNone(KeyCode::A));
        EXPECT_FALSE(IsKeyPressed(KeyCode::A));
        EXPECT_TRUE(IsKeyReleased(KeyCode::A));
        EXPECT_FALSE(IsKeyHeld(KeyCode::A));
    }

    {
        SCOPED_TRACE("Frame 4");
        provider->Poll();

        EXPECT_FALSE(IsKeyDown(KeyCode::A));
        EXPECT_TRUE(IsKeyNone(KeyCode::A));
        EXPECT_FALSE(IsKeyPressed(KeyCode::A));
        EXPECT_FALSE(IsKeyReleased(KeyCode::A));
        EXPECT_FALSE(IsKeyHeld(KeyCode::A));
    }
}

TEST_F(InputProviderTest, KeyStateLifecycleOccurLast){
    {
        SCOPED_TRACE("Frame 0");
        provider->Poll();

        EXPECT_FALSE(IsKeyDown(KeyCode::A));
        EXPECT_TRUE (IsKeyNone(KeyCode::A));
        EXPECT_FALSE(IsKeyPressed(KeyCode::A));
        EXPECT_FALSE(IsKeyReleased(KeyCode::A));
        EXPECT_FALSE(IsKeyHeld(KeyCode::A));

        // input occured, but applied at next frame.
        provider->PressKey(KeyCode::A);
    }

    {
        SCOPED_TRACE("Frame 1");
        provider->Poll();

        EXPECT_TRUE(IsKeyDown(KeyCode::A));
        EXPECT_FALSE(IsKeyNone(KeyCode::A));
        EXPECT_TRUE(IsKeyPressed(KeyCode::A));
        EXPECT_FALSE(IsKeyReleased(KeyCode::A));
        EXPECT_FALSE(IsKeyHeld(KeyCode::A));
    }

    {
        SCOPED_TRACE("Frame 2");
        provider->Poll();

        EXPECT_TRUE(IsKeyDown(KeyCode::A));
        EXPECT_FALSE(IsKeyNone(KeyCode::A));
        EXPECT_FALSE(IsKeyPressed(KeyCode::A));
        EXPECT_FALSE(IsKeyReleased(KeyCode::A));
        EXPECT_TRUE(IsKeyHeld(KeyCode::A));

        provider->ReleaseKey(KeyCode::A);
    }

    {
        SCOPED_TRACE("Frame 3");
        provider->Poll();

        EXPECT_FALSE(IsKeyDown(KeyCode::A));
        EXPECT_FALSE(IsKeyNone(KeyCode::A));
        EXPECT_FALSE(IsKeyPressed(KeyCode::A));
        EXPECT_TRUE(IsKeyReleased(KeyCode::A));
        EXPECT_FALSE(IsKeyHeld(KeyCode::A));
    }

    {
        SCOPED_TRACE("Frame 4");
        provider->Poll();

        EXPECT_FALSE(IsKeyDown(KeyCode::A));
        EXPECT_TRUE(IsKeyNone(KeyCode::A));
        EXPECT_FALSE(IsKeyPressed(KeyCode::A));
        EXPECT_FALSE(IsKeyReleased(KeyCode::A));
        EXPECT_FALSE(IsKeyHeld(KeyCode::A));
    }
}
