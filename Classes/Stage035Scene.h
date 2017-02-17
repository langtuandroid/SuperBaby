//
//  Stage035Scene.h
//  superBaby
//
//  Created by Administrator on 15/7/6.
//
//

#ifndef __superBaby__Stage035Scene__
#define __superBaby__Stage035Scene__

#include "StageScene.h"

class Actor;
class StageData;
class Stage035Scene : public StageScene
{
public:
    Stage035Scene();
    virtual ~Stage035Scene();
    
public:
    static Stage035Scene* createWithStageData(const StageData*);
    
public:
    virtual RoundBase* loadRound();
    virtual bool isPopShareNode(const unsigned int roundIndex){return roundIndex == 5;};
};

#endif /* defined(__superBaby__Stage035Scene__) */
