#include <iostream>
#include "userinterface.h"
#include "myworld.h"
#include <Imagine/Graphics.h>
#include "graphics.h"
#include "MyShape.h"

enum CurrentAction{Free,Drag,Poly,Circle};
MyShape* findTargetShape(Vector2 v){
    using World::ShapeList;
    for (vector<MyShape*>::iterator i=ShapeList.begin(); i<ShapeList.end(); i++) {
        if((*i)->shapeType==POLYGON){
            if(PointInPolygon(*static_cast<const MyPolygon*>(*i),v))return *i;
        }
    }
    return 0;//NULL
}

class Functionality{
private:
    World::Button *pButton;
public:
    Functionality(World::Button& b){pButton=&b;}
    void turnOn(){pButton->state=true;start();}
    void turnOff(){pButton->state=false;terminate();}
    virtual void terminate()=0;
    virtual void start()=0;
    virtual void takeAction()=0;
    bool buttonTouched(int x,int y){
        return x>=pButton->x&&y>pButton->y&&x<=pButton->x+pButton->w-1&&y<=pButton->y+pButton->h-1;
    }

};
class FunDrag:public Functionality{
private:
    MyShape *pShape;
    Vector2 dragPointRelative;
    Vector2 mousePoint;
public:
    FunDrag():Functionality(World::buttonDrag){pShape=0;}
    void start(){}
    void terminate(){pShape=0;}
    void takeAction(){
        Event ev;
        do{
            getEvent(0,ev);
            if(ev.type==EVT_BUT_ON&&ev.button==1){
                Vector2 v;
                coordTranslateInverse(ev.pix.x(),ev.pix.y(),v.x,v.y);
                pShape=findTargetShape(v);
                if(pShape==0)continue;
                Vector2 c=pShape->center;
                double angle=-pShape->angle;
                Matrix2 m_rotate(cos(angle),-sin(angle),sin(angle),cos(angle));
                dragPointRelative=m_rotate*(v-c);
                mousePoint=v;
            }else if(ev.type==EVT_MOTION&&pShape!=0){
                Event ev1;
                getEvent(0,ev1);
                while(ev1.type==EVT_MOTION){
                    ev=ev1;
                    getEvent(0,ev1);
                }
                if(ev1.type!=EVT_NONE)unGetEvent(ev1);
                Vector2 v;
                coordTranslateInverse(ev.pix.x(),ev.pix.y(),v.x,v.y);
                mousePoint=v;

            }else if(ev.type==EVT_BUT_OFF){
                pShape=0;
            }
            std::cout<<"count"<<std::endl;
        }while(ev.type!=EVT_NONE);
        if(pShape!=0){
            Vector2 c=pShape->center;
            double angle=pShape->angle;
            Matrix2 m_rotate(cos(angle),-sin(angle),sin(angle),cos(angle));
            Vector2 dragPointAbsolute=c+m_rotate*dragPointRelative;
            myDrawLine(dragPointAbsolute.x,dragPointAbsolute.y,mousePoint.x,mousePoint.y,BLACK);
            pShape->force+=10*(mousePoint-dragPointAbsolute);
            pShape->torque+=10*crossProd2D((dragPointAbsolute-c),(mousePoint-dragPointAbsolute));
        }
    }
};
class FunPoly:public Functionality{
private:
    vector<Vector2> vList;
public:
    FunPoly():Functionality(World::buttonPoly){}
    void start(){vList.clear();}
    void terminate(){vList.clear();}
    void takeAction(){
        Event ev;
        double x,y;
        do{
            getEvent(0,ev);
            if(ev.type==EVT_BUT_ON&&ev.button==1){
                coordTranslateInverse(ev.pix.x(),ev.pix.y(),x,y);
                vList.push_back(Vector2(x,y));
            }else if(ev.type==EVT_BUT_ON&&ev.button==3){
                double mass=1;
                World::ShapeList.push_back(new MyPolygon(mass,vList));
                vList.clear();
            }
        }while(ev.type!=EVT_NONE);
        if(!vList.empty()){
            int xint1,yint1,xint2,yint2;
            for(vector<Vector2>::iterator i=vList.begin();i<vList.end()-1;i++){
                coordTranslate(i->x,i->y,xint1,yint1);
                coordTranslate((i+1)->x,(i+1)->y,xint2,yint2);
                myDrawLine(i->x,i->y,(i+1)->x,(i+1)->y,RED);
                myDrawPoint(*i,2,RED);
            }
        }
    }
};
class FunCircle:public Functionality{
private:
public:
    FunCircle():Functionality(World::buttonCircle){}
    void start(){}
    void terminate(){}
    void takeAction(){}
};
void getLastEvent(Event& lastEv){
    Event e;
    getEvent(0,e);
    lastEv=e;
    while(e.type!=EVT_NONE){
        lastEv=e;
        getEvent(0,e);
    };
}
void flushAllEvents(){
    Event ev;
    getEvent(0,ev);
    while(ev.type!=EVT_NONE)getEvent(0,ev);
}

void eventTreatment(){
    using namespace World;
    static CurrentAction currentAction=Free;
    static FunDrag funDrag;
    static FunPoly funPoly;
    static FunCircle funCircle;
    Event ev;
    getEvent(0,ev);
    if(ev.type==EVT_BUT_ON&&ev.button==1){//按钮检测
        int x=ev.pix.x(),y=ev.pix.y();
        if(funDrag.buttonTouched(x,y)&&currentAction!=Drag){
            currentAction=Drag;
            funDrag.turnOn();funPoly.turnOff();funCircle.turnOff();
            return;
        }else if(funPoly.buttonTouched(x,y)&&currentAction!=Poly){
            currentAction=Poly;
            funPoly.turnOn();funDrag.turnOff();funCircle.turnOff();
            return;
        }else if(funCircle.buttonTouched(x,y)&&currentAction!=Circle){
            currentAction=Circle;
            funCircle.turnOn();funPoly.turnOff();funDrag.turnOff();
            return;
        }
    }
    unGetEvent(ev);
    switch (currentAction) {//发送消息
    case Drag:
        funDrag.takeAction();
        break;
    case Poly:
        funPoly.takeAction();
        break;
    case Circle:
        funCircle.takeAction();
        break;
    default:
        break;
    }
    flushAllEvents();

}
