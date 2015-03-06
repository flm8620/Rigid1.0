#include "multicollision.h"
#include <list>
#include <stack>
#include "lcp.h"
#include <iostream>
#include <iomanip>
#include "graphics.h"
using namespace std;
vector<vector<int> > groupingGraph(Graph G);
void getMinFromProjection(const vector<Vector2>& v,Vector2 n,double& min,vector<Vector2>::const_iterator& min_i);
void clipSegment(Vector2& l1,Vector2& l2,Vector2 clipPoint,Vector2 n);
bool SATPolygon(const MyPolygon& a,const MyPolygon& b,Vector2& n,Vector2& point1,Vector2& point2,int& numOfContact,double& depth);
bool collide_testPolygonPolygon(int sa,int sb,const MyPolygon& a,const MyPolygon& b,vector<ContactPoint>& newContacts,Vector2& MTV);
bool foundCollision(int i,int j,const MyShape* pShape1,const MyShape* pShape2,vector<ContactPoint>& newContacts,Vector2& MTV);
void addContacts(vector<ContactPoint>& Contacts,vector<vector<int> >& Shape_Contacts,const vector<ContactPoint>& newContacts);
double Calcul_A(int i,int J,int K,const vector<MyShape *>& Shapes,const vector<ContactPoint>& Contacts);
double Calcul_b(int i,int J,const vector<MyShape *>& Shapes,const vector<ContactPoint>& Contacts);
void addImpulse(vector<MyShape *>& Shapes,const ContactPoint& contact,double fi);
void drawContacts(const vector<ContactPoint>& Contacts);
void PositionalCorrection(MyShape &A, MyShape &B, Vector2 MTV);



void multiCollision(vector<MyShape *>& Shapes)
{
    //go through all pairs of "Shapes" to find "ContactPoints"
    int N=Shapes.size();
    vector<ContactPoint> Contacts;
    vector<vector<int> > Shape_Contacts(N);
    Graph G(N);
    int i,j;
    i=0;
    Vector2 MTV;//MTV=Minimum Translation Vector
    for(vector<MyShape *>::iterator it_i=Shapes.begin();it_i<Shapes.end()-1;it_i++){
        j=i+1;
        for(vector<MyShape *>::iterator it_j=it_i+1;it_j<Shapes.end();it_j++){
            vector<ContactPoint> newContacts;
            if(foundCollision(i,j,*it_i,*it_j,newContacts,MTV)){
                G.linkTwoNodes(i,j);
                addContacts(Contacts,Shape_Contacts,newContacts);
                PositionalCorrection(**it_i,**it_j,MTV);
            }
            j++;
        }
        i++;
    }
    drawContacts(Contacts);
    vector<vector<int> > groups=groupingGraph(G);

    if(groups.empty())return;//no collision, exit

    int solvingTimesCount=0;
    //go through every contactGroup, solve the LCP problem
    for(vector<vector<int> >::iterator it=groups.begin();it<groups.end();it++){
        //if(it->size()<=1)continue
        list<int> ls;//set of index of contactPoints in this group
        for(vector<int>::iterator i=it->begin();i<it->end();i++){
            for(vector<int>::iterator j=Shape_Contacts[*i].begin();j<Shape_Contacts[*i].end();j++){
                ls.push_back(*j);
            }
        }
        ls.sort();
        ls.unique();
        int n=ls.size();
        solvingTimesCount+=n;
        vector<int> num(Contacts.size(),-1);//"num" is the inverse of "ls"
        int k=0;
        for(list<int>::iterator ittt=ls.begin();ittt!=ls.end();ittt++){
            num[*ittt]=k;
            k++;
        }
        Matrix A(n,n,0);
        Matrix b(n,1,0);

        for(vector<int>::iterator si=it->begin();si<it->end();si++){//si: index of "Shapes"
            //go through all pairs (J,K ) of contactPoints of Shape[si]


            for(vector<int>::iterator J=Shape_Contacts[*si].begin();J<Shape_Contacts[*si].end();J++){
                for(vector<int>::iterator K=J+1;K<Shape_Contacts[*si].end();K++){
                    double temp=Calcul_A(*si,*J,*K,Shapes,Contacts);
                    //non-diagonal elements of A
                    A.set(num[*J],num[*K])+=temp;
                    A.set(num[*K],num[*J])+=temp;
                }
            }
            for(vector<int>::iterator J=Shape_Contacts[*si].begin();J<Shape_Contacts[*si].end();J++){
                double temp=Calcul_A(*si,*J,*J,Shapes,Contacts);
                //diagonal elements of A
                A.set(num[*J],num[*J])+=temp;
            }
            for(vector<int>::iterator j=Shape_Contacts[*si].begin();j<Shape_Contacts[*si].end();j++){
                b.set(num[*j],0)+=Calcul_b(*si,*j,Shapes,Contacts);
            }
        }
        //cout<<"A="<<endl;
        //A.print();
        //cout<<"b="<<endl;
        //b.print();
        Matrix f(n,1,0);
        try{
        LCPsolver(A,(1+0.2)*b,f);
        }catch(int error){
            continue;
        }
        //cout<<"f="<<endl;
        //f.print();
        k=0;
        for(list<int>::iterator ittt=ls.begin();ittt!=ls.end();ittt++){
            addImpulse(Shapes,Contacts[*ittt],f.get(k,0));
            k++;
        }
    }
    //cout<<"Contacts solved: "<<solvingTimesCount<<endl;
}
Graph::Graph(int n):G(n,n,0),size(n)
{

}

std::vector<int> Graph::getNeighbours(int node)
{
    std::vector<int> n;
    for(int i=0;i<size;i++){
        if(G.get(node,i)==1)n.push_back(i);
    }
    return n;
}

vector<vector<int> > groupingGraph(Graph G)
{
    vector<vector<int> > outPut;
    vector<bool> record;//to record if one node is grouped
    record.assign(G.getsize(),false);//mark all the nodes as false
    int i;// start from the i_th node, find the largest subtree
    for(i=0;i<G.getsize();i++){
        if(record[i]==true)continue;
        outPut.push_back(vector<int>());
        std::vector<int> &vi=outPut.back();
        std::stack<int> s;//stack s, to stock those nodes which are waiting to be grouped
        s.push(i);
        record[i]=true;//to mark that the i_th node is grouped
        while(!s.empty()){
            int t=s.top();
            s.pop();
            vi.push_back(t);

            std::vector<int> n=G.getNeighbours(t);
            for (std::vector<int>::iterator it = n.begin() ; it != n.end(); ++it){
                if(record[*it]==false){//if this neighbour is already grouped (must be the current groupe), do nothing.
                    s.push(*it);
                    record[*it]=true;//to mark that the it_th node is grouped
                }
            }
        }
        //if current groupe has only one shape, that means there is no collision for this shape. throw it.
        if(vi.size()==1)outPut.pop_back();
    }
    return outPut;
}
void getMinFromProjection(const vector<Vector2>& v,Vector2 n,double& min,vector<Vector2>::const_iterator& min_i){
    min=1e15;
    double proj;
    for(vector<Vector2>::const_iterator i=v.begin();i<v.end();i++){
        proj=n*(*i);
        if(proj<min){
            min=proj;
            min_i=i;
        }
    }
}
void clipSegment(Vector2& l1,Vector2& l2,Vector2 clipPoint,Vector2 n){
    double a,b,c;
    Vector2 t=l2-l1;
    a=n*clipPoint;b=l1*n;c=l2*n;
    if(a>b){
        l1+=(a-b)/(c-b)*t;
    }
    if(a>c){
        l2-=(a-c)/(b-c)*t;
    }
}

bool SATPolygon(const MyPolygon& a,const MyPolygon& b,Vector2& n,Vector2& point1,Vector2& point2,int& numOfContact,double& depth){
    double mina,minb,maxa,maxb,d1,d2,temp;
    depth=1e15;
    bool referenceIsA=false;
    Vector2 refP1,refP2;
    Vector2 incP1,incP2;
    Vector2 normal;
    //reference A
    vector<Vector2>::const_iterator min_i,max_i,p,p_prev,p_next;
    for(vector<Vector2>::const_iterator i=a.vertex.begin();i<a.vertex.end()-1;i++){
        normal=*(i+1)-*i;
        rotate90clockwise(normal);
        getMinFromProjection(b.vertex,normal,minb,min_i);
        maxa=normal*(*i);
        temp=maxa-minb;
        if(temp<0)return false;
        else if(temp<depth){
            n=normal;
            depth=temp;
            referenceIsA=true;
            refP1=*i;refP2=*(i+1);
            p=min_i;
        }

    }
    //A.end
    vector<Vector2>::const_iterator i=a.vertex.end()-1,j=a.vertex.begin();
    normal=*j-*i;
    rotate90clockwise(normal);
    getMinFromProjection(b.vertex,normal,minb,min_i);
    maxa=normal*(*i);
    temp=maxa-minb;
    if(temp<0)return false;
    else if(temp<depth){
        n=normal;
        depth=temp;
        referenceIsA=true;
        refP1=*i;refP2=*j;
        p=min_i;
    }

    //reference B
    for(vector<Vector2>::const_iterator i=b.vertex.begin();i<b.vertex.end()-1;i++){
        normal=*(i+1)-*i;
        rotate90clockwise(normal);
        getMinFromProjection(a.vertex,normal,mina,min_i);
        maxb=normal*(*i);
        temp=maxb-mina;
        if(temp<0)return false;
        else if(temp<depth){
            n=normal;
            depth=temp;
            referenceIsA=false;
            refP1=*i;refP2=*(i+1);
            p=min_i;
        }

    }
    //B.end
    i=b.vertex.end()-1;
    j=b.vertex.begin();
    normal=*j-*i;
    rotate90clockwise(normal);
    getMinFromProjection(a.vertex,normal,mina,min_i);
    maxb=normal*(*i);
    temp=maxb-mina;
    if(temp<0)return false;
    else if(temp<depth){
        n=normal;
        depth=temp;
        referenceIsA=false;
        refP1=*i;refP2=*j;
        p=min_i;
    }

    //p_prev,p_next
    if(referenceIsA){
        if(p==b.vertex.end()-1){
            p_next=b.vertex.begin();
            p_prev=p-1;
        }
        else if(p==b.vertex.begin()){
            p_prev=b.vertex.end()-1;
            p_next=p+1;
        }else{
            p_prev=p-1;
            p_next=p+1;
        }
    }else{
        if(p==a.vertex.end()-1){
            p_next=a.vertex.begin();
            p_prev=p-1;
        }
        else if(p==a.vertex.begin()){
            p_prev=a.vertex.end()-1;
            p_next=p+1;
        }
        else{
            p_prev=p-1;
            p_next=p+1;
        }
    }
    //
    if(abs(n*(*p-*p_next))>abs(n*(*p-*p_prev))){
        incP1=*p;
        incP2=*p_prev;
    }else{
        incP1=*p;
        incP2=*p_next;
    }
    //incP1 incP2 refP1 refP2 n
    clipSegment(incP1,incP2,refP1,refP2-refP1);
    clipSegment(incP1,incP2,refP2,refP1-refP2);
    n=n/sqrt(n*n);
    if(incP1*n>refP1*n){
        point1=incP2;
        numOfContact=1;
        n=referenceIsA?-n:n;
        return true;
    }else if(incP2*n>refP1*n){
        point1=incP1;
        numOfContact=1;
        n=referenceIsA?-n:n;
        return true;
    }else{
        point1=incP1;
        point2=incP2;
        numOfContact=2;
        n=referenceIsA?-n:n;
        return true;
    }


}

bool collide_testPolygonPolygon(int sa,int sb,const MyPolygon& a,const MyPolygon& b,vector<ContactPoint>& newContacts,Vector2& MTV){
    //return newContacts,MTV=Minimum Translation Vector
    Vector2 n,p1,p2;
    int numOfContact;
    double depth;
    if(SATPolygon(a,b,n,p1,p2,numOfContact,depth)){
        MTV=depth*n;
        newContacts.push_back(ContactPoint(p1,n,sa,sb));
        if(numOfContact==2){
            newContacts.push_back(ContactPoint(p2,n,sa,sb));
        }
        return true;
    }else{
        return false;
    }
}
ContactPoint::ContactPoint(Vector2 position, Vector2 normalvec, int s1, int s2):
    pos(position),n(normalvec),shape1(s1),shape2(s2)
{

}

bool foundCollision(int i,int j,const MyShape* pShape1,const MyShape* pShape2,vector<ContactPoint>& newContacts,Vector2& MTV){
    //MTV=Minimum Translation Vector
    if (TestAABBAABB(pShape1->aabb,pShape2->aabb)){

        if (pShape1->shapeType==POLYGON&&pShape2->shapeType==POLYGON) {
            if(collide_testPolygonPolygon(i,j,*static_cast<const MyPolygon*>(pShape1),*static_cast<const MyPolygon*>(pShape2),newContacts,MTV) ){
                return true;
            }
        }
        return false;
    }
    return false;
}
void addContacts(vector<ContactPoint>& Contacts,vector<vector<int> >& Shape_Contacts,const vector<ContactPoint>& newContacts){
    int k;
    for(vector<ContactPoint>::const_iterator it=newContacts.begin();it<newContacts.end();it++){
        Contacts.push_back(*it);
        k=Contacts.size()-1;
        Shape_Contacts[it->shape1].push_back(k);
        Shape_Contacts[it->shape2].push_back(k);
    }
}

double Calcul_A(int i,int J,int K,const vector<MyShape *>& Shapes,const vector<ContactPoint>& Contacts){
    //i--Shape
    //J,K--contact point
    const ContactPoint& cJ=Contacts[J];
    const ContactPoint& cK=Contacts[K];
    const MyShape& s=*Shapes[i];
    Vector2 nJ,nK;
    if(cJ.shape1==i)nJ=cJ.n;else nJ=-cJ.n;
    if(cK.shape1==i)nK=cK.n;else nK=-cK.n;
    Vector2 pj=cJ.pos-s.center;
    Vector2 pk=cK.pos-s.center;
    double L=crossProd2D(pj,nJ);
    Vector2 vel=nJ*s.invMass;
    double vel_ang=L*s.invI;
    rotate90anticlockwise(pk);
    Vector2 vel2=vel_ang*pk;
    return (vel+vel2)*nK;
}
double Calcul_b(int i,int J,const vector<MyShape *>& Shapes,const vector<ContactPoint>& Contacts){
    //i--Shape
    //J--contact point
    const ContactPoint& cJ=Contacts[J];
    Vector2 p=cJ.pos-Shapes[i]->center;
    Vector2 vel=Shapes[i]->vel;
    rotate90anticlockwise(p);
    Vector2 vel2=Shapes[i]->angVel*p;
    if(cJ.shape1==i)
        return cJ.n*(vel+vel2);
    else
        return -cJ.n*(vel+vel2);
}
void addImpulse(vector<MyShape *>& Shapes,const ContactPoint& contact,double fi){
    MyShape* s1=Shapes[contact.shape1];
    MyShape* s2=Shapes[contact.shape2];
    const Vector2& n=contact.n;
    const Vector2& pos=contact.pos;
    s1->vel+=n*fi*s1->invMass;
    s2->vel-=n*fi*s2->invMass;
    Vector2 p1=pos-s1->center;
    Vector2 p2=pos-s2->center;
    double L1=fi*crossProd2D(p1,n);
    double L2=-fi*crossProd2D(p2,n);
    s1->angVel+=L1*s1->invI;
    s2->angVel+=L2*s2->invI;
}
void drawContacts(const vector<ContactPoint>& Contacts){
    for(vector<ContactPoint>::const_iterator i=Contacts.begin();i<Contacts.end();i++){
        myDrawPoint(i->pos);
    }
}
void PositionalCorrection(MyShape &A, MyShape &B, Vector2 MTV) {//MTV=Minimum Translation Vector
    const double percent=0.2; // usually 20% to 80%
    const double sq_slop=0.01*0.01;
    double ma_inv=A.invMass, mb_inv=B.invMass;
    if (A.fixToGround) ma_inv=0;
    if (B.fixToGround) mb_inv=0;
    Vector2 correction=MTV/(ma_inv+mb_inv)*percent;
    //cout<<A.ID<<"vs"<<B.ID<<"correction："<<correction.x<<'\t'<<correction.y<<endl;
    A.center+=ma_inv*correction;
    B.center-=mb_inv*correction;
}











