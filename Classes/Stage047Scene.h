//
//  Stage047Scene.h
//  superBaby
//
//  Created by Administrator on 15/8/5.
//
//

#ifndef __superBaby__Stage047Scene__
#define __superBaby__Stage047Scene__

#include "StageScene.h"

class Actor;
class StageData;
class Stage047Scene : public StageScene
{
public:
    Stage047Scene();
    virtual ~Stage047Scene();
    
public:
    static Stage047Scene* createWithStageData(const StageData*);
    
public:
    virtual RoundBase* loadRound();
    virtual bool isPopShareNode(const unsigned int roundIndex){return roundIndex == 4;};
};


#endif /* defined(__superBaby__Stage047Scene__) */
