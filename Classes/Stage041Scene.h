//
//  Stage041Scene.h
//  superBaby
//
//  Created by Administrator on 15/7/28.
//
//

#ifndef __superBaby__Stage041Scene__
#define __superBaby__Stage041Scene__

#include "StageScene.h"

class Actor;
class StageData;
class Stage041Scene : public StageScene
{
public:
    Stage041Scene();
    virtual ~Stage041Scene();
    
public:
    static Stage041Scene* createWithStageData(const StageData*);
    
public:
    virtual RoundBase* loadRound();
    virtual bool isPopShareNode(const unsigned int roundIndex){return roundIndex == 6;};
};

#endif /* defined(__superBaby__Stage041Scene__) */
