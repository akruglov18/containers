#include "HashMap.h"
#include <gtest/gtest.h>

namespace {

TEST(HashMap, init) {
    HashMap<int, int> hashMap(10);
    hashMap.insert(5, 13);
    hashMap.insert(3, 12);
    hashMap.erase(5);
    ASSERT_EQ(12, hashMap.read(3));
}

}
