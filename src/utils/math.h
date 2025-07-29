#ifndef MATH_INCLUDE
#define MATH_INCLUDE

#define EPSILON 0.000001

#include <iostream>

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
    Tuple operator*(const Tuple& other) const;
    Tuple operator*(float scalar) const;
    //Tuple operator*(int scalar) const;
    Tuple operator/(float scalar) const;
    Tuple operator-() const;

    Tuple cross(const Tuple& other) const;
};

Tuple Point(float x, float y, float z);
Tuple Vector(float x, float y, float z);
Tuple Color(float r, float g, float b);

Tuple operator*(float scalar, const Tuple& tuple);
Tuple operator*(int scalar, const Tuple& tuple);

std::ostream& operator<<(std::ostream& os, const Tuple& p);

#endif

