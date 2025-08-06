#include "camera.h"
#include <math.h>

#include "../utils/ray.h"
void Camera::set_pixel_size(int hsize, int vsize, double field_of_view) {
    double half_view = tan(field_of_view / 2);
    double aspect = (float)(hsize) / vsize;

    if(aspect >= 1){ 
        half_width = half_view;
        half_height = half_view / aspect; 
    } else {
        half_width = half_view * aspect;
        half_height = half_view;
    }
    pixel_size = (half_width * 2 ) / hsize;
}

Camera::Camera(int hsize, int vsize, double field_of_view, Matrix transform) : hsize(hsize), vsize(vsize), field_of_view(field_of_view), transform(transform) {
    set_pixel_size(hsize, vsize, field_of_view);
}
Camera::Camera(int hsize, int vsize, double field_of_view) : hsize(hsize), vsize(vsize), field_of_view(field_of_view), transform(IDENTITY_MATRIX) {
    set_pixel_size(hsize, vsize, field_of_view);
}
Camera::Camera() : transform(IDENTITY_MATRIX) {}

Camera::Camera(const Camera& other) : hsize(other.hsize), vsize(other.vsize), field_of_view(other.field_of_view), transform(other.transform), half_height(other.half_height), half_width(other.half_width), pixel_size(other.pixel_size) {}

Ray Camera::ray_for_pixel(int px, int py) const {
    double xoffset = (px + 0.5) * pixel_size;
    double yoffset = (py + 0.5) * pixel_size;

    double world_x = half_width - xoffset;
    double world_y = half_height - yoffset;

    Tuple pixel = transform.inverse() * Point(world_x, world_y, -1);
    Tuple origin = transform.inverse() * Point(0,0,0);
    Tuple direction = (pixel - origin).normalize();

    return Ray(origin, direction);
}