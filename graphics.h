#ifndef GRAPHICS_H
#define GRAPHICS_H
#include <Imagine/Graphics.h>
class MyShape;
class Vector2;
class MyPolygon;
void myDisplay();
void initialView(int width,int height);
void draw_each(MyShape* s);
void draw_axis();
void myDrawPoint(const Vector2&,int radius=5,Imagine::Color col=Imagine::RED);
void DrawPolygon(MyPolygon &p);
void myDrawLine(double x1,double y1,double x2,double y2,Imagine::Color col);
void coordTranslate(double x,double y,int& x_onScreen,int& y_onScreen);
void coordTranslateInverse(int x_onScreen, int y_onScreen, double& realX, double& realY);
#endif // GRAPHICS_H
