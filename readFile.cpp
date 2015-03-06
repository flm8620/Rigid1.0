#include "readFile.h"
using namespace std;
void readTwoNumberForProperty(ifstream& inFile,const string& nameOfProperty,double& x,double& y){
    string line;
    if (getline(inFile,line)) {
        cout<<line<<endl;
        line.erase(line.end()-1);
        istringstream input_istring(line);
        input_istring>>x>>y;
        if (!input_istring)
        {
            cout<<"fail to load property: "<<nameOfProperty<<endl;
            exit(1);
        }
    }
    else {
        cout<<"fail to load property: "<<nameOfProperty<<endl;
        exit(1);
    }
}
void readOneNumberForProperty(ifstream& inFile,const string& nameOfProperty,double& x){
    string line;
    if (getline(inFile,line)) {
        cout<<line<<endl;
        line.erase(line.end()-1);
        istringstream input_istring(line);
        input_istring>>x;
        if (!input_istring)
        {
            cout<<"fail to load property: "<<nameOfProperty<<endl;
            exit(1);
        }
    }
    else {
        cout<<"fail to load property: "<<nameOfProperty<<endl;
        exit(1);
    }
}

void readFile(vector<MyShape*> &ShapeList) {
    ifstream inFile("data_classic.txt",std::ios::binary);
    string s;
    ShapeType shapeType;
    bool setVertex, setMove, setVel, setAngVel, setFix,\
            setRotate, setMoveTo, setMass, setRestitution,setFriction;
    vector<Vector2 > vt;
    Vector2 move, vel, moveTo;
    bool fix;
    double rotate, angVel, mass, restitution,friction;
    while (getline(inFile,s)) {
        cout<<s<<endl;
        //remove the "\n"
        //s.erase(s.end()-1);
        //ignore the comment line
        if (s[0]=='/'&&s[1]=='/') continue;
        //read the beginning of a declaration: shapetype
        if (s.find("Polygon")!=string::npos) {
            shapeType=POLYGON;
        }
        else if (s.find("Circle")!=string::npos) {
            shapeType=CIRCLE;
        }
        else if (s.find("Rectangle")!=string::npos) {
            shapeType=RECTANGLE;
        }
        else if (s.empty()||s.compare("\r")==0) {
            continue;
        }
        else {
            cout<<"Read file error: Unknown shape type!"<<endl;
        }
        //initialize properties
        vt.clear();
        move.x=move.y=0;
        vel.x=vel.y=0;
        moveTo.x=moveTo.y=0;
        fix=false;
        rotate=0;
        angVel=0;
        mass=1;
        restitution=0.5;
        friction=0.3;
        setVertex=setMove=setVel=setAngVel=setFix=setRotate=\
                setMoveTo=setRestitution=setFriction=false;
        //start to read those properties
        while (getline(inFile,s)) {
            cout<<s<<endl;
            //s.erase(s.end()-1);
            //ignore the comment line
            if (s[0]=='/'&&s[1]=='/') continue;
            //check if the declaration of current shape terminates
            else if (s.find("end")!=string::npos) {
                switch (shapeType) {
                case POLYGON:
                    ShapeList.push_back(new MyPolygon(mass,vt));
                    break;
                }
                //create one shape
                MyShape& s=*ShapeList.back();
                if (setMove) s.move(move.x,move.y);
                if (setMoveTo) s.moveTo(moveTo.x,moveTo.y);
                s.angVel=angVel;
                if (setRotate) s.rotate(rotate);
                s.vel=vel;
                if(fix){
                    s.fixToGround=true;
                    s.invMass=0;
                    s.mass=0;
                    s.I=0;
                    s.invI=0;
                    s.vel.x=s.vel.y=0;
                }
                s.restitution=restitution;
                s.friction=friction;
                break;
            }

            else if (s.find("vertex:")!=string::npos) {
                setVertex=true;
                double x,y;
                //read those vertice coordinates
                while (getline(inFile,s)) {
                    cout<<s<<endl;
                    //s.erase(s.end()-1);
                    if (s.length()==0) break;
                    istringstream input_istring(s);
                    input_istring>>x>>y;
                    //if there is no more coordinates, store vertice, break this loop
                    if (!input_istring) {
                        if (vt.size()>=3) {
                            inFile.seekg(-1-(long) s.length(),inFile.cur);
                            break;
                        }
                        else {
                            cout<<"number of vertex is less than 3 !";
                            exit(1);
                        }
                    }
                    vt.push_back(Vector2(x,y));
                }
            }
            else if (s.find("vel:")!=string::npos) {
                setVel=true;
                readTwoNumberForProperty(inFile,"vel",vel.x,vel.y);
            }
            else if (s.find("mass:")!=string::npos) {
                setMass=true;
                readOneNumberForProperty(inFile,"mass",mass);
            }
            else if (s.find("move:")!=string::npos) {
                setMove=true;
                readTwoNumberForProperty(inFile,"move",move.x,move.y);
            }
            else if (s.find("ang_vel:")!=string::npos) {
                setAngVel=true;
                readOneNumberForProperty(inFile,"ang_vel",angVel);
            }
            else if (s.find("moveto:")!=string::npos) {
                setMoveTo=true;
                readTwoNumberForProperty(inFile,"moveto",moveTo.x,moveTo.y);
            }
            else if (s.find("rotate:")!=string::npos) {
                setRotate=true;
                readOneNumberForProperty(inFile,"rotate",rotate);
            }
            else if (s.find("fix:")!=string::npos) {
                setFix=true;
                double x;
                readOneNumberForProperty(inFile,"fix",x);
                if (x==1) fix=true;
            }
            else if (s.find("restitution:")!=string::npos) {
                setRestitution=true;
                readOneNumberForProperty(inFile,"restitution",restitution);
            }
            else if (s.find("friction:")!=string::npos) {
                setFriction=true;
                readOneNumberForProperty(inFile,"friction",friction);
            }
            else {
                cout<<"Unknown property"<<endl;
                exit(1);
            }
        }
    }

}
