#ifndef MATH_INCLUDE
#define MATH_INCLUDE

#define EPSILON 0.000001

struct Tuple {
    float x, y, z, w;
    Tuple(float x, float y, float z, float w);
    Tuple();
    bool isPoint() const;
    bool isVector() const;
    bool operator==(const Tuple& other) const {
        return (x - other.x < EPSILON) && (y - other.y < EPSILON) && (z - other.z < EPSILON) && (w- other.w < EPSILON);
    }
};

struct Vector : public Tuple {
    Vector(float x, float y, float z);
};

struct Point : public Tuple {
    Point(float x, float y, float z);
};

#endif

