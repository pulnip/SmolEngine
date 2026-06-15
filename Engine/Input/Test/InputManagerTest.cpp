#include <gtest/gtest.h>
#include "InputManager.hpp"
#include "InputConfig.hpp"
#include "KeyCode.hpp"
#include "MockInputProvider.hpp"

using namespace Smol;

static MockInputProvider& GetInputProvider(){
    static MockInputProvider singleton;

    return singleton;
}
static InputManager& GetInputManager(){
    using enum KeyCode;

    static InputManager singleton(InputConfig{
        .mappings = {
            ActionInfo{
                .name = "Action",
                .mappings = {
                    KeyBinding{
                        .cond = A
                    },
                }
            }
        }
    }, &GetInputProvider());

    return singleton;
}

class InputManagerTest: public ::testing::Test{
protected:
    void SetUp() override{
        GetInputProvider().Reset();
    }
};

static u32 actionCallCounter = 0;

struct TestActor{
private:
    InputAction action;

public:
    TestActor(){
        action = GetInputManager().BindAction(
            "Action", TriggerEvent::Started,
            [&](InputValue v){ OnAction(v); }
        );
    }
    ~TestActor() = default;

    void OnAction(InputValue){
        ++actionCallCounter;
    }
};

TEST_F(InputManagerTest, ActionBindAndUnbind){
    auto& provider = GetInputProvider();
    auto& manager = GetInputManager();

    {
        SCOPED_TRACE("Frame 0");
        provider.NewFrame();

        manager.NewFrame();

        // No KeyState  Poll
        EXPECT_EQ(actionCallCounter, 0);
    }
    {
        SCOPED_TRACE("Frame 1");
        provider.NewFrame();
        provider.PressKey(KeyCode::A);

        TestActor actor;
        manager.NewFrame();

        // Press A Applied
        EXPECT_EQ(actionCallCounter, 1);
    }
    {
        SCOPED_TRACE("Frame 2");
        provider.NewFrame();
        provider.ReleaseKey(KeyCode::A);

        TestActor actor;
        manager.NewFrame();

        // Release A Applied
        EXPECT_EQ(actionCallCounter, 1);
    }
}
