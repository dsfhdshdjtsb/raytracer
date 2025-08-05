#include <gtest/gtest.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <memory>
#include "../src/utils/light.h"
#include "../src/scene/world.h"
#include "../src/utils/math.h"
#include "../src/utils/ray.h"
#include "../src/utils/shape.h"

TEST(World, intersections) {
    World w = DefaultWorld();

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

TEST(World, Lighting) {
    World w = DefaultWorld();
    w.light = PointLight(Color(1,1,1), Point(0,0.25, 0));
    Ray r(Point(0,0,0), Vector(0,0,1));
    std::shared_ptr<Shape> s = w.objects[0];

    Intersection i(0.5, s);
    Computations comps = i.prepare_computations(r);
    Tuple c = w.shade_hit(comps);
    EXPECT_EQ(Color(0.90498, 0.90498, 0.90498), c);

}

TEST(World, ColorAt) {
    World w = DefaultWorld();

    Ray r(Point(0,0,-5), Vector(0,0,1));

    Tuple c = w.color_at(r);
    EXPECT_EQ(Color(0.38066, 0.47583, 0.2855), c);

    w.objects[0]->material.ambient = 1;
    w.objects[1]->material.ambient = 1;

    r = Ray(Point(0,0,0.75), Vector(0,0,-1));
    c = w.color_at(r);
    EXPECT_EQ(Color(1,1,1), c);
}

