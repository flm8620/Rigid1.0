#ifndef MYSHAPE_H
#define MYSHAPE_H
#include <iostream>
#include <math.h>
#include <vector>
#include <string>
#include <algorithm>
#include <stdio.h>
#include "algebra.h"


const double PI=3.141926536;
enum ShapeType{POLYGON,CIRCLE,RECTANGLE};
class AABB
{
public:
    double left, right, top, bottom;
};
class MyShape
{
public:
    Vector2 center, vel, center_last, vel_last, force;
    static int IDcount;
    int ID;
    ShapeType shapeType;
    double angle;
    double angVel; //angular velocity
    double torque;
    double mass;
    double area;
    double invMass;
    double I; //Moment of inertia
    double invI;
    double restitution;
    double friction;
    bool fixToGround;
    AABB aabb;
    void move(double dx, double dy);
    void moveTo(double x, double y);
    virtual void rotate(double degree)=0;
    virtual void draw()=0;
    MyShape(ShapeType st, double x, double y, double m, double vx, double vy, double ang_v, double a);
    void makeMove(double dt);
    virtual void calc_vertex()=0;
    virtual void calc_AABB()=0;
private:
};

class MyCircle: public MyShape
{
public:
    void rotate(double degree);
    void draw();
    void calc_AABB();
    void calc_vertex()
    {
    }
    double radius;
    MyCircle(double r, double x, double y, double m, double vx=0, double vy=0);
private:
};

class MyPolygon: public MyShape
{
public:
    int vertex_n;
    std::vector<Vector2> vertexFixed;//must be in the anticlockwise order
    std::vector<Vector2> vertex;
    void rotate(double degree);
    void draw();
    void calc_AABB();
    MyPolygon(double m, std::vector<Vector2> &vt, double vx=0, double vy=0);
    void calc_vertex();
private:
    Vector2 calc_center() const;
    void vertex_relativization();
    void calc_I();
};
bool TestAABBAABB(const AABB&, const AABB&);
bool PointInPolygon(const MyPolygon&, const Vector2&);
bool TriangleIsCCW(const Vector2&, const Vector2&, const Vector2&);
double ORIENT2D(const Vector2&, const Vector2&, const Vector2&);
int WhichSideIsNearest(const MyPolygon&, const Vector2&);
double SqDistPointLine(const Vector2& p, const Vector2& a1, const Vector2& a2);

#endif

