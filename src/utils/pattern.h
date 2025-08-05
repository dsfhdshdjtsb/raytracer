#ifndef PATTERN_INCLUDE
#define PATTERN_INCLUDE

#include "math.h"

struct Pattern {
    Matrix transform;

    Pattern();

    virtual Tuple color_at(const Tuple& point) const;
};

struct Solid : public Pattern {
    Tuple a; //color

    Solid(const Tuple& a); 
    Tuple color_at(const Tuple& point) const;
};

struct Stripe : public Pattern {
    Tuple a;
    Tuple b;

    Stripe();
    Stripe(const Tuple& a, const Tuple& b);
    Tuple color_at(const Tuple& point) const;
};

struct Gradient: public Pattern {
    Tuple a;
    Tuple b;
    Gradient(const Tuple& a, const Tuple& b);
    Tuple color_at(const Tuple& point) const;
};

struct Ring : public Pattern {

    Tuple a;
    Tuple b;
    Ring(const Tuple& a, const Tuple& b);
    Tuple color_at(const Tuple& point) const;
};

struct Checkers : public Pattern {
    Tuple a;
    Tuple b;
    Checkers(const Tuple& a, const Tuple& b);
    Tuple color_at(const Tuple& point) const;
};

#endif