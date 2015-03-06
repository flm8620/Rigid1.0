#include "graphics.h"
#include "MyShape.h"
#include "myworld.h"
#include <Imagine/Graphics.h>
using namespace Imagine;
namespace View{
int winWidth,winHeight;
double viewZoneLeft,viewZoneRight,viewZoneUp,viewZoneBottom;
double viewWidth,viewHeight;
double ratioWidth,ratioHeight;
}
void coordTranslate(double x,double y,int& x_onScreen,int& y_onScreen){
    using namespace View;
    x_onScreen=(x-viewZoneLeft)*ratioWidth;
    y_onScreen=(viewZoneUp-y)*ratioHeight;
}
void coordTranslateInverse(int x_onScreen, int y_onScreen, double& realX,double& realY)
{
    using namespace View;
    realX=x_onScreen/ratioWidth+viewZoneLeft;
    realY=viewZoneUp-y_onScreen/ratioHeight;
}
void drawButton(World::Button b){
    if(b.state){
        fillRect(b.x,b.y,b.w,b.h,RED);
    }else{
        fillRect(b.x,b.y,b.w,b.h,BLUE);
    }
    drawString(b.x,b.y,b.name,BLACK);

}

void myDisplay()
{
    using World::ShapeList;
    using World::buttonDrag;
    using World::buttonPoly;
    using World::buttonCircle;

    noRefreshBegin();
    setBackGround(WHITE);
    for_each(ShapeList.begin(),ShapeList.end(),draw_each);
    draw_axis();
    drawButton(buttonDrag);
    drawButton(buttonPoly);
    drawButton(buttonCircle);
    noRefreshEnd();

}
void draw_each(MyShape* s)
{
    if(s->shapeType==POLYGON){

        DrawPolygon(*static_cast<MyPolygon*>(s));
    }
    //drawAABB
}
void draw_axis()
{
    //drawAxis
}
void myDrawPoint(const Vector2& p, int radius, Color col) {
    int x,y;
    coordTranslate(p.x,p.y,x,y);
    fillCircle(x,y,radius,col);
}
void DrawPolygon(MyPolygon &p){
    if(p.shapeType!=POLYGON){cout<<"drawpolygon need polygon"<<endl;exit(0);}
    vector<Vector2 >::iterator v;
    p.calc_vertex();
    for (v=p.vertex.begin(); v<p.vertex.end()-1; ++v){
        myDrawLine((*v).x,(*v).y,(*(v+1)).x,(*(v+1)).y, BLACK);

    }
    myDrawLine(p.vertex.back().x,p.vertex.back().y,p.vertex.front().x,p.vertex.front().y, BLACK);
}
void myDrawLine(double x1,double y1,double x2,double y2,Color col){
    using namespace View;
    int xx1,xx2,yy1,yy2;
    coordTranslate(x1,y1,xx1,yy1);
    coordTranslate(x2,y2,xx2,yy2);
    drawLine(xx1,yy1,xx2,yy2,col);
}


void initialView(int width, int height)
{
    using namespace View;
    winWidth=width;
    winHeight=height;
    viewZoneLeft=-10;
    viewZoneRight=10;
    viewZoneUp=10;
    viewZoneBottom=-10;
    viewWidth=viewZoneRight-viewZoneLeft;
    viewHeight=viewZoneUp-viewZoneBottom;
    ratioWidth=winWidth/viewWidth;
    ratioHeight=winHeight/viewHeight;
}



