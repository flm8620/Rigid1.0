#include "MyShape.h"
using namespace std;
int MyShape::IDcount=0;
double ORIENT2D(const Vector2& a, const Vector2& b, const Vector2& c)
{
    return (a.x-c.x)*(b.y-c.y)-(b.x-c.x)*(a.y-c.y);
}
bool TriangleIsCCW(const Vector2& a, const Vector2& b, const Vector2& c)
{
    return ORIENT2D(a,b,c)>0;
}
bool TestAABBAABB(const AABB& a, const AABB& b)
{
    if (a.left>b.right||a.right<b.left)
        return false;
    if (a.bottom>b.top||a.top<b.bottom)
        return false;
    return true;

}
bool PointInPolygon(const MyPolygon& s, const Vector2& p)
{
    int low=0, high=s.vertex_n;
    do
    {
        int mid=(low+high)/2;
        if (TriangleIsCCW(s.vertex[0],s.vertex[mid],p))
            low=mid;
        else
            high=mid;
    } while (low+1<high);
    if (low==0||high==s.vertex_n)
        return false;
    return TriangleIsCCW(s.vertex[low],s.vertex[high],p);
}
int WhichSideIsNearest(const MyPolygon &s, const Vector2& p)
{
    vector<Vector2 >::const_iterator i, j;
    const vector<Vector2 > &v=s.vertex;
    const int& n=s.vertex_n;
    double dist_min,dist;
    int min_num=n-1;
    dist=dist_min=SqDistPointLine(p,v[n-1],v[0]);
    for (i=v.begin(); i<v.end()-1; i++)
    {
        j=i+1;
        dist=SqDistPointLine(p,(*i),(*j));
        dist<dist_min?dist_min=dist,min_num=i-v.begin():1;
    }
    return min_num;
}

double SqDistPointLine(const Vector2& p, const Vector2& a1,
        const Vector2& a2){
    return (p-a1)*(p-a1)-((p-a1)*(a2-a1))*((p-a1)*(a2-a1))/((a2-a1)*(a2-a1));
}

MyShape::MyShape(ShapeType st,double x, double y, double m, double vx, double vy,double ang_v,double a) :
        angle(0), angVel(ang_v)
{
    shapeType=st;
    center.x=x;center.y=y;
    vel.x=vx;vel.y=vy;
    force.x=0;force.y=0;
    torque=0;
    ID=IDcount;
    IDcount++;
    mass=m;
    area=a;
    invMass=1/m;
    restitution=0.8;
    friction=0.0;
    fixToGround=false;
}
void MyShape::move(double dx, double dy)
{
    center.x+=dx;
    center.y+=dy;
    calc_vertex();
    calc_AABB();
}
void MyShape::moveTo(double x, double y)
{
    center.x=x;
    center.y=y;
    calc_vertex();
    calc_AABB();
}
MyCircle::MyCircle(double r, double x, double y, double m, double vx, double vy) :
        MyShape(CIRCLE,x,y,m,vx,vy,0,3.1415927*r*r), radius(r)
{
    I=m*r*r/2;
}
void MyCircle::rotate(double degree)
{
    angle+=degree;
}
void MyCircle::draw()
{
    //draw
}
MyPolygon::MyPolygon(double m, vector<Vector2> &vt, double vx, double vy) :
        MyShape(POLYGON,0.0,0.0,m,vx,vy,0,0)
{
    vertex_n=vt.size();
    vertex.resize(vertex_n);
    copy(vt.begin(),vt.end(),back_inserter(vertexFixed));
    center=calc_center();
    vertex_relativization();
    calc_vertex();
    calc_I();
}
void MyPolygon::vertex_relativization()
{
    for (vector<Vector2 >::iterator v=vertexFixed.begin(); v<vertexFixed.end();
            v++)
        *v-=center;
}
void MyPolygon::rotate(double degree)
{
    angle+=degree;
    calc_vertex();
}
Vector2 MyPolygon::calc_center() const
{
    vector<Vector2 >::const_iterator v;
    int n=vertex_n;
    n-=2;
    double sumx=0, sumy=0, sumarea=0;
    double x[3], y[3];
    v=vertexFixed.begin();
    x[0]=(*v).x;
    y[0]=(*v).y;
    v++;
    x[1]=(*v).x;
    y[1]=(*v).y;
    while (n--)
    {
        v++;
        x[2]=(*v).x;
        y[2]=(*v).y;
        double s=((x[1]-x[0])*(y[2]-y[0])-(x[2]-x[0])*(y[1]-y[0]))/2.0;
        sumx+=((x[0]+x[1]+x[2])*s);
        sumy+=((y[0]+y[1]+y[2])*s);
        sumarea+=s;

        x[1]=x[2];
        y[1]=y[2];
    }
    Vector2 result(sumx/sumarea/3,sumy/sumarea/3);
    return result;
}
void MyPolygon::draw()
{

//draw

}
void MyPolygon::calc_vertex()
{
    Matrix2 m_rotate(cos(angle),-sin(angle),sin(angle),cos(angle));
    Vector2 t;
    int i=0;
    vector<Vector2 >::iterator v;

    for (v=vertexFixed.begin(); v<vertexFixed.end(); ++v)
    {
        t=m_rotate*(*v);

        vertex[i]=t+center;
        ++i;

    }
}
void MyPolygon::calc_I()
{
    if(fixToGround){
        I=0;invI=0;return;
    }
    double sum1=0, sum2=0, temp;
    int j;
    vector<Vector2>&v=vertexFixed;
    for (int i=0; i<vertex_n; i++)
    {
        j=(i+1)%vertex_n;
        temp=v[i].x*v[j].y-v[j].x*v[i].y;
        sum1+=temp*(v[j]*v[j]+v[j]*v[i]+v[i]*v[i]);
        sum2+=temp;
    }
    area=sum2;
    I=mass/6*sum1/sum2;
    invI=1/I;
}
void MyShape::makeMove(double dt)
{
    if(fixToGround)return;
    Vector2 acc=force*invMass;
    double ang_acc=torque*invI;
    vel+=acc*dt/2;
    center+=vel*dt;
    vel+=acc*dt/2;
    angVel+=ang_acc*dt/2;
    angle+=angVel*dt;
    angVel+=ang_acc*dt/2;
    calc_vertex();

}
void MyPolygon::calc_AABB()
{
    vector<Vector2 >::iterator i=vertex.begin();
    aabb.left=aabb.right=(*i).x;
    aabb.top=aabb.bottom=(*i).y;
    for (; i<vertex.end(); i++)
    {
        aabb.right=(*i).x>aabb.right ? (*i).x : aabb.right;
        aabb.left=(*i).x<aabb.left ? (*i).x : aabb.left;
        aabb.top=(*i).y>aabb.top ? (*i).y : aabb.top;
        aabb.bottom=(*i).y<aabb.bottom ? (*i).y : aabb.bottom;
    }
}
void MyCircle::calc_AABB()
{
    aabb.left=center.x-radius;
    aabb.right=center.x+radius;
    aabb.top=center.y+radius;
    aabb.bottom=center.y-radius;
}
