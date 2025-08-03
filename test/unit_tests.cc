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

TEST(Cameras, Render) {
    World w;
    w.camera = Camera(11,11,M_PI/2);
    Tuple from = Point(0,0,-5);
    Tuple to = Point(0,0,0);
    Tuple up = Point(0,1,0);
    w.camera.transform = ViewTransform(from, to, up);
    Canvas image = w.render();
    EXPECT_EQ(Color(0.38066, 0.47583, 0.2855), image[5][5]);
}