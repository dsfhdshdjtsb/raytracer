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

TEST(MathTest, Opers) {
    Vector a(4,-4, 0);
    Vector t(4, -4, 3);

    EXPECT_EQ(a+t, Tuple(8, -8, 3, 0));

    EXPECT_EQ(a-t, Tuple(0, 0, -3, 0));

    Vector b(1,2,3);

    EXPECT_EQ(-b, Vector(-1,-2,-3));
    EXPECT_EQ(2 * b, Vector(2,4,6));
    std::cout << (a.dot(t)) << std::endl;
    EXPECT_EQ(32, a.dot(t));
}
TEST(MathTest, Cross) {
    Vector a = Vector(1,2,3);
    Vector b = Vector(2,3,4);
    EXPECT_EQ(Vector(1, -2, 1) ,b.cross(a) );
}
