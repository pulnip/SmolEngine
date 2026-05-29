#include <vector>
#include <gtest/gtest.h>
#include "CoreFWD.hpp"
#include "Semantics.hpp"
#include "SlotMap.hpp"

using namespace Smol;

struct LifetimeTracker{
    static u32 constructed;
    static u32 destructed;
    static u32 moved;

    static void reset(){
        constructed = destructed = moved = 0;
    }

    u32 value;

    LifetimeTracker(u32 v = 0)
        :value(v)
    {
        ++constructed;
    }
    ~LifetimeTracker(){
        ++destructed;
    }
    LifetimeTracker(LifetimeTracker&& other) noexcept
        :value(other.value)
    {
        ++moved;
    }
    LifetimeTracker& operator=(LifetimeTracker&& other) noexcept{
        value = other.value;
        ++moved;
        return *this;
    }

    SMOL_DECLARE_NON_COPYABLE(LifetimeTracker)
};

u32 LifetimeTracker::constructed = 0;
u32 LifetimeTracker::destructed = 0;
u32 LifetimeTracker::moved = 0;

class SlotMapTest: public ::testing::Test{
protected:
    SlotMap<i32> intMap;
    SlotMap<Str> stringMap;

    void SetUp() override{
        LifetimeTracker::reset();
    }
};

TEST_F(SlotMapTest, DefaultConstruction){
    SlotMap<i32> map;
    EXPECT_EQ(map.size(), 0);
    EXPECT_EQ(map.capacity(), 0);
}

TEST_F(SlotMapTest, PushAndFind){
    auto handle = intMap.push(42);

    EXPECT_EQ(intMap.size(), 1);

    auto ptr = intMap.find(handle);
    ASSERT_NE(ptr, nullptr);
    EXPECT_EQ(*ptr, 42);
}

TEST_F(SlotMapTest, PushAndGet){
    auto handle = intMap.push(42);

    EXPECT_EQ(intMap.get(handle), 42);
}

TEST_F(SlotMapTest, EmplaceString){
    auto handle = stringMap.emplace("hello world");

    EXPECT_EQ(stringMap.size(), 1);
    EXPECT_EQ(stringMap.get(handle), "hello world");
}

TEST_F(SlotMapTest, MultiplePush){
    auto h1 = intMap.push(1);
    auto h2 = intMap.push(2);
    auto h3 = intMap.push(3);

    EXPECT_EQ(intMap.size(), 3);
    EXPECT_EQ(intMap.get(h1), 1);
    EXPECT_EQ(intMap.get(h2), 2);
    EXPECT_EQ(intMap.get(h3), 3);
}

TEST_F(SlotMapTest, RemoveBasic){
    auto handle = intMap.push(42);
    EXPECT_TRUE(intMap.remove(handle));
    EXPECT_EQ(intMap.size(), 0);
}

TEST_F(SlotMapTest, RemoveInvalidatesHandle){
    auto handle = intMap.push(42);
    intMap.remove(handle);

    EXPECT_EQ(intMap.find(handle), nullptr);
}

TEST_F(SlotMapTest, RemoveAndReuse){
    auto h1 = intMap.push(1);
    auto h2 = intMap.push(2);

    intMap.remove(h1);

    auto h3 = intMap.push(3);

    EXPECT_EQ(h3.GetIndex(), h1.GetIndex());
    EXPECT_NE(h3.GetGeneration(), h1.GetGeneration());

    EXPECT_EQ(intMap.get(h3), 3);
    EXPECT_EQ(intMap.get(h2), 2);

    EXPECT_EQ(intMap.find(h1), nullptr);
}

TEST_F(SlotMapTest, RemoveExpiredHandleReturnsFalse){
    auto h1 = intMap.push(1);
    intMap.remove(h1);

    EXPECT_FALSE(intMap.remove(h1));
}

TEST_F(SlotMapTest, RemoveOutOfRangeReturnsFalse){
    GenericHandle<int> invalidHandle(999, 0);
    EXPECT_FALSE(intMap.remove(invalidHandle));
}

TEST_F(SlotMapTest, Clear){
    intMap.push(1);
    intMap.push(2);
    intMap.push(3);

    intMap.clear();

    EXPECT_EQ(intMap.size(), 0);
    EXPECT_EQ(intMap.capacity(), 3);
}

TEST_F(SlotMapTest, ClearInvalidatesAllHandles){
    auto h1 = intMap.push(1);
    auto h2 = intMap.push(2);

    intMap.clear();

    EXPECT_EQ(intMap.find(h1), nullptr);
    EXPECT_EQ(intMap.find(h2), nullptr);
}

TEST_F(SlotMapTest, PushAfterClear){
    auto h1 = intMap.push(1);
    intMap.clear();

    auto h2 = intMap.push(2);

    EXPECT_EQ(intMap.size(), 1);
    EXPECT_EQ(intMap.get(h2), 2);
    EXPECT_EQ(intMap.find(h1), nullptr);
}

TEST_F(SlotMapTest, Reserve){
    intMap.reserve(100);

    EXPECT_EQ(intMap.size(), 0);
    EXPECT_GE(intMap.capacity(), 100);
}

TEST_F(SlotMapTest, ReserveDoesNotShrink){
    intMap.reserve(100);
    intMap.reserve(50);

    EXPECT_GE(intMap.capacity(), 100);
}

TEST_F(SlotMapTest, IteratorEmpty){
    EXPECT_EQ(intMap.begin(), intMap.end());
}

TEST_F(SlotMapTest, IteratorBasic){
    intMap.push(1);
    intMap.push(2);
    intMap.push(3);

    std::vector<int> values;
    for(const auto& v: intMap){
        values.push_back(v);
    }

    EXPECT_EQ(values.size(), 3);

    EXPECT_EQ(values[0], 1);
    EXPECT_EQ(values[1], 2);
    EXPECT_EQ(values[2], 3);
}

TEST_F(SlotMapTest, IteratorSkipsFreeSlots){
    auto h1 = intMap.push(1);
    intMap.push(2);
    intMap.push(3);

    intMap.remove(h1);

    std::vector<int> values;
    for(const auto& v: intMap){
        values.push_back(v);
    }

    EXPECT_EQ(values.size(), 2);
    EXPECT_EQ(values[0], 2);
    EXPECT_EQ(values[1], 3);
}

TEST_F(SlotMapTest, IteratorWithHoles){
    auto h1 = intMap.push(1);
    auto h2 = intMap.push(2);
    auto h3 = intMap.push(3);
    auto h4 = intMap.push(4);
    auto h5 = intMap.push(5);

    intMap.remove(h2);
    intMap.remove(h4);

    std::vector<int> values;
    for(const auto& v: intMap){
        values.push_back(v);
    }

    EXPECT_EQ(values.size(), 3);
    EXPECT_EQ(values[0], 1);
    EXPECT_EQ(values[1], 3);
    EXPECT_EQ(values[2], 5);
}

TEST_F(SlotMapTest, ConstIterator){
    intMap.push(1);
    intMap.push(2);

    i32 sum = 0;
    for(const auto& i: intMap){
        sum += i;
    }

    EXPECT_EQ(sum, 3);
}

TEST_F(SlotMapTest, IteratorModification){
    intMap.push(1);
    intMap.push(2);

    for(auto& v: intMap){
        v *= 10;
    }

    std::vector<i32> values;
    for(const auto& v: intMap){
        values.push_back(v);
    }

    EXPECT_EQ(values[0], 10);
    EXPECT_EQ(values[1], 20);
}

TEST(SlotMapLifetimeTest, DestructorCallsElementDestructors){
    LifetimeTracker::reset();

    {
        SlotMap<LifetimeTracker> map;
        map.reserve(3);

        map.emplace(1);
        map.emplace(2);
        map.emplace(3);
    }

    EXPECT_EQ(LifetimeTracker::destructed, 3);
}

TEST(SlotMapLifetimeTest, RemoveCallsDestructor){
    LifetimeTracker::reset();

    SlotMap<LifetimeTracker> map;
    auto h = map.emplace(42);

    auto destructedBefore = LifetimeTracker::destructed;
    map.remove(h);

    EXPECT_GT(LifetimeTracker::destructed, destructedBefore);
}

TEST(SlotMapLifetimeTest, ClearCallsDestructors){
    LifetimeTracker::reset();

    SlotMap<LifetimeTracker> map;
    map.emplace(1);
    map.emplace(2);
    map.emplace(3);

    auto destructedBefore = LifetimeTracker::destructed;
    map.clear();

    EXPECT_EQ(LifetimeTracker::destructed - destructedBefore, 3);
}

TEST_F(SlotMapTest, ManyInsertRemoveCycles){
    std::vector<GenericHandle<i32>> handles;

    for(i32 cycle=0; cycle<10; ++cycle){
        handles.clear();

        for(i32 i=0; i<100; ++i){
            handles.push_back(intMap.push(i));
        }

        for(i32 i=0; i<100; i+=2){
            intMap.remove(handles[i]);
        }

        EXPECT_EQ(intMap.size(), 50);

        for(i32 i=1; i<100; i+=2){
            EXPECT_EQ(intMap.get(handles[i]), i);
        }

        intMap.clear();
    }
}

TEST_F(SlotMapTest, HandleStaysValidAfterOtherOperations){
    auto h1 = intMap.push(1);

    auto h2 = intMap.push(2);
    auto h3 = intMap.push(3);
    intMap.remove(h2);
    auto h4 = intMap.push(4);

    EXPECT_NE(intMap.find(h1), nullptr);
    EXPECT_EQ(intMap.get(h1), 1);
}

TEST_F(SlotMapTest, GenerationOverflow){
    auto handle = intMap.push(1);

    for(i32 i=0; i<1000; ++i){
        intMap.remove(handle);
        handle = intMap.push(i);
    }

    EXPECT_EQ(intMap.get(handle), 999);
}

TEST(SlotMapMoveOnlyTest, UniquePtr){
    SlotMap<RAII<i32>> map;

    auto h = map.push(std::make_unique<i32>(42));

    EXPECT_EQ(**map.find(h), 42);

    map.remove(h);
    EXPECT_EQ(map.find(h), nullptr);
}
