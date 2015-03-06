#ifndef MYWORLD_H
#define MYWORLD_H

#include <math.h>
#include <vector>
#include <iostream>

#include "MyShape.h"
#include "readFile.h"
#include "algebra.h"
#include <time.h>
#include <algorithm>
#include <stdio.h>
#include <string>
#include <Imagine/Graphics.h>
using namespace Imagine;

namespace World {
extern vector<MyShape*> ShapeList;
extern clock_t currentTime, lastTime,lastFrameTime;
extern double simulateSpeed;
extern double dt;
extern Vector2 gravityAcc;
extern clock_t frameTime;//除以模拟速度放大比例
struct Button{
    int x,y,w,h;
    bool state;
    string name;
};
extern Button buttonDrag,buttonPoly,buttonCircle;
}



void oneStep();
void calc_force(MyShape* s);
void calc_vel(MyShape* s);
void collide_test();
bool collide_testPolygonPolygon(MyPolygon& a, MyPolygon& b, Vector2& point, Vector2& Line1, Vector2& Line2,
		MyShape *&shape_inserter, MyShape *&shape_inserted);
const Vector2 GetNearestPointToLine(const Vector2& c, const Vector2& a, const Vector2& b);
void collide_act(MyShape &inserter, MyShape &inserted, Vector2 collide_point, Vector2 insert_point);
void PositionalCorrection(MyShape &A, MyShape &B, Vector2 penetrationDepthN);
void stop_all_shape();
void myinit();
const Vector2 GetNearestPointToLine(const Vector2& c, const Vector2& a, const Vector2& b);

#endif
