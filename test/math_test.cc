#include <gtest/gtest.h>
#include "../src/utils/math.h"

TEST(MathTest, TupleWithW1IsPoint) {
    Tuple a(4.3, -4.2, 3.1, 1.0);
    EXPECT_FLOAT_EQ(a.x, 4.3);
    EXPECT_FLOAT_EQ(a.y, -4.2);
    EXPECT_FLOAT_EQ(a.z, 3.1);
    EXPECT_FLOAT_EQ(a.w, 1.0);
    EXPECT_TRUE(a.isPoint());
    EXPECT_FALSE(a.isVector());
}
TEST(MathTest, TupleWithW0IsVector) {
    Tuple a(4.3, -4.2, 3.1, 0.0);
    EXPECT_FLOAT_EQ(a.x, 4.3);
    EXPECT_FLOAT_EQ(a.y, -4.2);
    EXPECT_FLOAT_EQ(a.z, 3.1);
    EXPECT_FLOAT_EQ(a.w, 0.0);
    EXPECT_FALSE(a.isPoint());
    EXPECT_TRUE(a.isVector());
}
TEST(MathTest, PointHasW1) {
    Point a(4, -4, 3);
    Tuple t(4, -4, 3, 1);
    EXPECT_EQ(a,t);
}
TEST(MathTest, VectorHasW0) {
    Vector a(4, -4, 3);
    Tuple t(4, -4, 3, 0);
    EXPECT_EQ(a,t);
}