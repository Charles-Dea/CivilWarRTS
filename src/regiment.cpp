#include<AL/al.h>
#include<iostream>
#include<random>
#include<math.h>
#include"cavalry_regiment.hpp"
using namespace std;
bool Regiment::regCanFire(Unit*target){
    if(findDistance(target->xPosition,target->yPosition,nullptr,nullptr)<=range)
        return regCanSee(target);
    return false;
}
bool Regiment::regCanSee(Unit*target){
    double relativeAngle=findRelativeAngle(
            target->xPosition,
            target->yPosition,
            nullptr
            );
    while(relativeAngle>=360){
        relativeAngle-=360;
    }
    while(relativeAngle<=360){
        relativeAngle+=360;
    }
    if(relativeAngle<=67.5||relativeAngle>=360-67.5)
        return unitCanSee(target);
    return false;
}
void Regiment::regSendOrder(OrderType order,float x,float y){
    if(order==halt){
        hasOrder=false;
    }else if(order!=commanderMarch){
        hasOrder=true;
        orderType=order;
        destinationX=x;
        destinationY=y;
        rotated=false;
    }else{
        printf("ERROR: invalid order given");
    }
}
float Regiment::regGetZ(float x,float y){
    return hillGetZ(hills,numHills,x,y)+0.1f;
}
void Regiment::processOrder(){
    if(!rotated){
        float angle;
        float relativeAngle=findRelativeAngle(destinationX,destinationY,&angle);
        while(relativeAngle<0.0f)
            relativeAngle+=MAX_DEGREES;
        while(relativeAngle>MAX_DEGREES)
            relativeAngle-=MAX_DEGREES;
        if(relativeAngle<=rotationSpeed||relativeAngle>=MAX_DEGREES-rotationSpeed){
            rotation=angle;
            rotated=true;
        }else if(relativeAngle<=180.0f){
            rotation+=rotationSpeed;
        }else{
            rotation-=rotationSpeed;
        }
    }else if(orderType==march){
        marchTowards(destinationX,destinationY);
    }
    firing=false;
}
void Regiment::fire(Unit*(*enemies)[],unsigned short enemyCount,float damage){
    unsigned short arrLength=0;
    for(unsigned short count=0;count<enemyCount;++count){
        if((*enemies)[count]){
 		    (*enemies)[count]->sorted=false;
            ++arrLength;
        }
    }
    if(arrLength>0){
        Unit*sortedEnemies[arrLength];
 		for(unsigned short count=0;count<enemyCount;++count){
     		Unit*target=nullptr;
            //initialize distance to impossibly high value to make sure ever enemy is sorted
 	        double distance=10000000000000.0;
         	for(unsigned short regCount=0;regCount<enemyCount;++regCount){
                if((*enemies)[regCount]){
                    float enemyXDistance=(*enemies)[regCount]->xPosition-xPosition;
 		            float enemyYDistance=(*enemies)[regCount]->yPosition-yPosition;
 	    	        double enemyDistance=sqrt(enemyXDistance*enemyXDistance+enemyYDistance*enemyYDistance);
         			if(enemyDistance<distance&&(*enemies)[regCount]->sorted==false){
 	    	    		target=(*enemies)[regCount];
 	        			distance=enemyDistance;
     				}
                }
 		    }
            if(target){
 	        	sortedEnemies[count]=target;
     	    	target->sorted=true;
 			    target->distance=distance;
            }
     	}
        bool fired=false;
 	    for(unsigned short count=0;count<arrLength&&!fired;++count){
            if((*this.*canFire)(sortedEnemies[count])){
                Unit*target=sortedEnemies[count];
                float heightDiff=hillGetZ(hills,numHills,xPosition,yPosition)-hillGetZ(hills,numHills,target->xPosition,target->yPosition)+1.0f;
                float trueDamage=damage;
                if(target->type==cavalry_regiment){
                    CavalryRegiment*t=(CavalryRegiment*)target;
                    if(!t->mounted)
                        trueDamage/=2.0f;
                }
                alSourcePlay(fireSound);
 	    	    target->size-=trueDamage*heightDiff;
     		    fired=true;
         		firing=true;
 	        	reload=0;
                neededReload=getReload();
 	        }
        }
    }
}
double Regiment::getReload(){
    static default_random_engine generator;
    static uniform_real_distribution<double>distribution(270.0,330.0);
    return distribution(generator);
}
