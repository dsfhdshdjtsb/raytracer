#include "../src/utils/math.h"
#include <iostream>
#include "../src/screen/canvas.h"
struct Env {
    Tuple wind;
    Tuple gravity;
    
    Env() : wind(-0.01, 0, 0, 0), gravity(0, -0.1, 0, 0) {}
};

struct Proj {
    Tuple position;
    Tuple velocity;

    Proj(Tuple pos, Tuple vel) : position(pos), velocity(vel) {}
};

Proj tick(Env env, Proj proj) {
    Tuple position = proj.position + proj.velocity;
    Tuple velocity = proj.velocity + env.gravity + env.wind;
    return Proj(position, velocity);
}
int main() {

    int i = 0;
    Proj proj(Point(0,1,0), Vector(1,1.8, 0).normalize() * 11.25);
    Env e;
    Canvas c(900, 550);
    Tuple c1 = Color(1, 0, 0);

    while(i++ < 1000) {
        proj = tick(e, proj);
        if(proj.position.x >= 0 && proj.position.x <= 900
        && proj.position.y >= 0 && proj.position.y <= 550) {
            c.writePixel(proj.position.x, 550 - proj.position.y, c1);
        }
        std::cout << proj.position << std::endl;
    }
    c.canvasToPPM();
}