//
//  Stage042Scene.h
//  superBaby
//
//  Created by Administrator on 15/7/30.
//
//

#ifndef __superBaby__Stage042Scene__
#define __superBaby__Stage042Scene__

#include "StageScene.h"

class Actor;
class StageData;
class Stage042Scene : public StageScene
{
public:
    Stage042Scene();
    virtual ~Stage042Scene();
    
public:
    static Stage042Scene* createWithStageData(const StageData*);
    
public:
    virtual RoundBase* loadRound();
    virtual bool isPopShareNode(const unsigned int roundIndex){return roundIndex == 6;};
};


#endif /* defined(__superBaby__Stage042Scene__) */
