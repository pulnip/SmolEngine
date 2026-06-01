#include <gtest/gtest.h>
#include "ClassRegistry.hpp"
#include "Object.hpp"
#include "Semantics.hpp"

class SimpleObject final: public Smol::Object{
    SMOL_OBJECT_BODY(SimpleObject)

public:
    SimpleObject() = default;
    ~SimpleObject() = default;
    SMOL_DECLARE_MOVE_ONLY(SimpleObject)
};

SMOL_OBJECT(SimpleObject)
SMOL_OBJECT_END(SimpleObject)

TEST(Reflection, SimpleObject){
    ASSERT_TRUE(_SimpleObjectRegistered);

    auto object = Smol::CreateObject("SimpleObject");
    ASSERT_TRUE(object != nullptr);

    auto simpleObject = dynamic_cast<SimpleObject*>(object.get());
    EXPECT_TRUE(simpleObject != nullptr);
}
