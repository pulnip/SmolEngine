#include <gtest/gtest.h>
#include "ClassRegistry.hpp"
#include "Object.hpp"
#include "Semantics.hpp"

class SimpleObject final: public Smol::Object{
    SMOL_OBJECT_BODY(SimpleObject, Smol::Object)

public:
    SimpleObject() = default;
    ~SimpleObject() = default;
    SMOL_DECLARE_MOVE_ONLY(SimpleObject)
};

SMOL_OBJECT(SimpleObject)
SMOL_OBJECT_END(SimpleObject)

TEST(Reflection, SimpleObject){
    ASSERT_TRUE(IsRegisteredSimpleObject);

    Smol::Str objectName = "SimpleObject";
    auto object = Smol::CreateObject(objectName);
    ASSERT_TRUE(object != nullptr);
    EXPECT_TRUE(object->GetClassName() == objectName);
    EXPECT_TRUE(object->IsA("Object"));
    EXPECT_TRUE(object->IsA(objectName));

    auto simpleObject = dynamic_cast<SimpleObject*>(object.get());
    EXPECT_TRUE(simpleObject != nullptr);
}
