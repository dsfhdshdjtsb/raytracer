#ifndef MATH_INCLUDE
#define MATH_INCLUDE

#define EPSILON 0.000001

struct Tuple {
    float x, y, z, w;
    Tuple(float x, float y, float z, float w);
    Tuple();
    bool isPoint() const;
    bool isVector() const;

    float magnitude() const;
    Tuple normalize() const;
    float dot(const Tuple& other) const;

    bool operator==(const Tuple& other) const;
    Tuple operator+(const Tuple& other) const;
    Tuple operator-(const Tuple& other) const;
    Tuple operator*(const float other) const;
    Tuple operator*(const int other) const;
    Tuple operator/(const float other) const;
    Tuple operator-() const;
};

// Global operators for scalar multiplication from the left
Tuple operator*(float scalar, const Tuple& tuple);
Tuple operator*(int scalar, const Tuple& tuple);

struct Vector : public Tuple {
    Vector(float x, float y, float z);
    Vector cross(const Vector& other) const;
};

struct Point : public Tuple {
    Point(float x, float y, float z);
};

#endif

