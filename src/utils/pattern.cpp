
#include <math.h>
#include "math.h"
#include "pattern.h"

Pattern::Pattern() : transform(IDENTITY_MATRIX){}
Tuple Pattern::color_at(const Tuple& point) const {
    return Color(1,1,1);
}

Solid::Solid(const Tuple& a) : a(a) {}

Tuple Solid::color_at(const Tuple& point) const {
    return a;
}

Stripe::Stripe(const Tuple& a, const Tuple& b) : a(a), b(b) {}
Stripe::Stripe() : a(Color(1,1,1)), b(Color(0,0,0)) {}

Tuple Stripe::color_at(const Tuple& point) const {
    Tuple cand = point;
    if(cand.x < 0) {
        cand.x -= 1;
    }
    if(((int) cand.x) % 2 == 0) {
        return a;
    } else {
        return b;
    }
}

Gradient::Gradient(const Tuple& a, const Tuple& b) : a(a), b(b) {}

Tuple Gradient::color_at(const Tuple& point) const {
    Tuple res = a + (b-a) * (point.x - floor(point.x));
    return res;
}

Ring::Ring(const Tuple& a, const Tuple& b) : a(a), b(b){}

Tuple Ring::color_at(const Tuple& point) const {
    if ((int) sqrt(point.x * point.x + point.y * point.y) % 2 == 0) {
        return a;
    } else {
        return b;
    }
}

Checkers::Checkers(const Tuple& a, const Tuple& b) : a(a), b(b){}

Tuple Checkers::color_at(const Tuple& point) const {
    if ( ( (int) point.x + (int) point.y + (int) point.z) % 2 == 0) {
        return a;
    } else {
        return b;
    }
}