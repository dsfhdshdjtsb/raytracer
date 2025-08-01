#include <gtest/gtest.h>
#include "../src/screen/canvas.h"
#include "../src/utils/math.h"
#include <string.h>
#include <iostream>
#include <fstream>

TEST(ScreenTest, initCanvas) {
    Canvas c(10, 10);

    for(int i = 0; i < 10; i++) {
        for(int j = 0; j < 10; j++) {
            EXPECT_EQ(Color(0,0,0), c.buffer[i][j]);
        }
    }

    c[1][1] = Color(1,0,0);
    EXPECT_EQ(Color(1,0,0), c.buffer[1][1]);
}

TEST(ScreenTest, ppmTest) {
    Canvas c(5,3);
    Tuple c1 = Color(1.5, 0, 0);
    Tuple c2 = Color(0, 0.5, 0);
    Tuple c3 = Color(-0.5, 0, 1);

    c[0][0] = c1;
    c[2][1] = c2;
    c[4][2] = c3;

    c.canvasToPPM();

    std::vector<std::string> exp ={ "P3",
                      "5 3",
                      "255",
                      "255 0 0 0 0 0 0 0 0 0 0 0 0 0 0 ",
                      "0 0 0 0 0 0 0 128 0 0 0 0 0 0 0 ",
                      "0 0 0 0 0 0 0 0 0 0 0 0 0 0 255 " };

    std::ifstream file("out.ppm");
    std::string line;
    int count = 0;
    while(std::getline(file, line)) {
        EXPECT_EQ(exp[count++], line);
    }

    //c = Canvas(10, 2);
    //Tuple c4 = Color(1, 0.8, 0.6);
    //for(int i = 0; i < 10; i++) {
        //for(int j = 0; j < 2; j++) {
            //c.writePixel(i, j, c4);
        //}
    //}
    //c.canvasToPPM();
}

