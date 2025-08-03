#ifndef CAMERA_INCLUDE
#define CAMERA_INCLUDE
#include "../utils/math.h"

struct Ray;

struct Camera{
    int hsize, vsize;
    float field_of_view, pixel_size, half_width, half_height;
    Matrix transform;

    Camera(int hsize, int vsize, float field_of_view, Matrix transform);
    Camera(int hsize, int vsize, float field_of_view);
    Camera();
    Camera(const Camera& other);
    void set_pixel_size(int hsize, int vsize, float field_of_view);
    Ray ray_for_pixel(int px, int py) const;
};

#endif