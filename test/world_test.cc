#include <gtest/gtest.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include "../src/scene/world.h"
#include "../src/utils/math.h"
#include "../src/utils/ray.h"
#include "../src/utils/shape.h"
#include "../src/utils/light.h"

TEST(World, intersections) {
    World w;

    Ray r(Point(0,0,-5), Vector(0,0,1));
    Intersections xs = w.intersect_world(r);
    EXPECT_EQ(4, xs.size());

    auto iter = xs.begin();
    EXPECT_EQ(4, (*iter).t );
    ++iter;
    EXPECT_EQ(4.5, (*iter).t);
    ++iter;
    EXPECT_EQ(5.5, (*iter).t);
    ++iter;
    EXPECT_EQ(6, (*iter).t);

}
