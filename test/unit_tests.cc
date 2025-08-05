#include <gtest/gtest.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <memory>
#include "../src/utils/light.h"
#include "../src/scene/world.h"
#include "../src/utils/math.h"
#include "../src/utils/ray.h"
#include "../src/utils/shape.h"
#include "../src/scene/camera.h"
#include "../src/screen/canvas.h"
#include "../src/utils/pattern.h"

TEST(Cameras, Construction) {
    Camera c(200, 125, M_PI / 2) ;
    EXPECT_EQ(true, c.pixel_size - 0.01 < 0.0001);
    c = Camera(125, 200, M_PI/2); 
    EXPECT_EQ(true, c.pixel_size - 0.01 < 0.0001);
}

TEST(Cameras, Pixels) {
    Camera c(201, 101, M_PI/2);
    Ray r = c.ray_for_pixel(100, 50);
    EXPECT_EQ(Point(0,0,0), r.origin);
    EXPECT_EQ(Vector(0,0,-1), r.direction);

    r = c.ray_for_pixel(0,0);
    EXPECT_EQ(Point(0,0,0), r.origin);
    EXPECT_EQ(Vector(0.66519,0.33259,-0.66851), r.direction);

    c.transform = Rotation_y(M_PI / 4) * Translation(0,-2,5);
    r = c.ray_for_pixel(100,50);
    EXPECT_EQ(Point(0,2,-5), r.origin);
    EXPECT_EQ(Vector(1,0,-1).normalize(), r.direction);
}

//TEST(Cameras, Render) {
    //World w = DefaultWorld();
    //w.camera = Camera(11,11,M_PI/2);
    //Tuple from = Point(0,0,-5);
    //Tuple to = Point(0,0,0);
    //Tuple up = Point(0,1,0);
    //w.camera.transform = ViewTransform(from, to, up);
    //Canvas image = w.render();
    //EXPECT_EQ(Color(0.38066, 0.47583, 0.2855), image[5][5]);
//}

TEST(World, isShadow) {
    World w = DefaultWorld();

    Tuple p = Point(0,10,0);
    EXPECT_EQ(false, w.is_shadowed(p));

    p = Point(10, -10, 10);
    EXPECT_EQ(true, w.is_shadowed(p));

    p = Point(-20, 20, -20);
    EXPECT_EQ(false, w.is_shadowed(p));

    p = Point(-2, 2, -2);
    EXPECT_EQ(false, w.is_shadowed(p));
}

TEST(World, shadows) {
    World w;
    w.light = PointLight(Color(1,1,1), Point(0,0,-10));
    std::shared_ptr<Shape> s1 = std::make_shared<Sphere>();
    std::shared_ptr<Shape> s2 = std::make_shared<Sphere>();
    s2->transform = Translation(0,0,10);

    w.objects.push_back(s1);
    w.objects.push_back(s2);

    Ray r(Point(0,0,5), Vector(0,0,1));
    Intersection i(4, s2);
    Computations comps = i.prepare_computations(r);
    Tuple c = w.shade_hit(comps);
    EXPECT_EQ(Color(0.1,0.1,0.1), c);
}

TEST(World, ShadowOffset) {
    Ray r(Point(0,0,-5), Vector(0,0,1));
    std::shared_ptr<Shape> s1 = std::make_shared<Sphere>();
    s1->transform = Translation(0,0,1);
    Intersection i(5, s1);
    Computations comps = i.prepare_computations(r) ;
    EXPECT_LT( comps.over_point.z, -EPSILON / 2);
    EXPECT_GT(comps.point.z, comps.over_point.z);
}

TEST(Patterns, Patterns) {
    std::shared_ptr<Pattern> pat = std::make_shared<Gradient>(Color(1,1,1), Color(0,0,0));
    EXPECT_EQ(pat->color_at(Point(0,0,0)), Color(1,1,1)); 
    EXPECT_EQ(pat->color_at(Point(0.5,0,0)), Color(0.5,0.5,0.5));
}