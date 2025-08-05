#include <gtest/gtest.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <memory.h>
#include "../src/utils/math.h"
#include "../src/utils/ray.h"
#include "../src/utils/shape.h"
#include "../src/utils/light.h"

TEST(Shapes, Normals) {
    Sphere s;

    s.set_transform(Translation(0,1,0));
    Tuple n = s.normal_at(Point(0, 1.70711, -.70711));
    EXPECT_EQ(Vector(0,.70711, -.70711), n);

    s = Sphere();
    s.set_transform(Scaling(1,0.5,1) * Rotation_z(M_PI / 5));
    n = s.normal_at(Point(0, sqrt(2) / 2, -sqrt(2) / 2));
    EXPECT_EQ(Vector(0,0.97014, -.24254), n);
}

TEST(Shapes, Shading) {
    Tuple pos = Point(0,0,0);

    Tuple eyev = Vector(0,0, -1) ;
    Tuple normalv = Vector(0,0,-1);
    PointLight light = PointLight(Color(1,1,1), Point(0,10,-10));
    Material mat;
    
    std::shared_ptr<Shape> sphere = std::make_shared<Sphere>();
    sphere->material = mat;
    Tuple result = mat.lighting(light, sphere, pos, eyev, normalv, false);
    EXPECT_EQ(Color(0.7364, 0.7364,0.7364), result); //light and eye angled

    light = PointLight(Color(1,1,1), Point(0,0,10));
    result = mat.lighting(light,sphere, pos, eyev, normalv, false);
    EXPECT_EQ(Color(0.1,0.1,0.1), result); //light behind mat

    eyev = Vector(0, 1, -1).normalize();
    light = PointLight(Color(1,1,1), Point(0,0,-10));
    result = mat.lighting(light,sphere, pos, eyev, normalv, false);
    EXPECT_EQ(Color(1,1,1), result);


    eyev = Vector(0, 1, -1).normalize();
    light = PointLight(Color(1,1,1), Point(0,0,-10));
    result = mat.lighting(light, sphere, pos, eyev, normalv, true);
    EXPECT_EQ(Color(0.1,0.1,0.1), result);
}


