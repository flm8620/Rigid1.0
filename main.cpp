#include <stdio.h>
#include <string>
#include <time.h>
#include "myworld.h"
#include "algebra.h"
#include "graphics.h"
#include <algorithm>
#include <math.h>
#include <ctime>
long myIdle()
{
    //using namespace World;
    clock_t timeGap;
    static double cumulativeTime=0;
    static int frameCount=0;
    World::currentTime=clock();
    timeGap=World::currentTime-World::lastTime;
    cumulativeTime+=timeGap;

    if(cumulativeTime>0.2*CLOCKS_PER_SEC){
        cumulativeTime=0.2*CLOCKS_PER_SEC;
        //cout <<time_acc<<endl;cout<< "-------------no output over 0.2 second---------"<<endl;
    }
    if (cumulativeTime>World::frameTime)
    {

        cumulativeTime-=World::frameTime;
        myDisplay();
        oneStep();
        frameCount++;
        if(frameCount==856){
            cout<<"stop"<<endl;
        }
        cout<<"framecount:"<<frameCount<<endl;
        cout<<"fps:"<<double(CLOCKS_PER_SEC)/(clock()-World::lastFrameTime)<<endl;

        //cout<<clock()<<endl;
        World::lastFrameTime=clock();
    }


    World::lastTime=World::currentTime;
    return (long)World::frameTime-cumulativeTime-(clock()-World::currentTime);
}

int main()
{
    myinit();//read file£¬initialize lastTime
    const int width=1024,height=1024;
    long time;
    initialView(width,height);
    openWindow(width,height);
    while(1){

        time=myIdle();

        if(time>0){
            //cout<<"time="<<time<<endl;
            //milliSleep(1000.*time/CLOCKS_PER_SEC);
        }



    }
    endGraphics();
    return 0;
}

