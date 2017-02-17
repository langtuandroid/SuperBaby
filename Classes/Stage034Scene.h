//
//  Stage034Scene.h
//  superBaby
//
//  Created by Administrator on 15/6/23.
//
//

#ifndef __superBaby__Stage034Scene__
#define __superBaby__Stage034Scene__

#include "StageScene.h"

class Actor;
class StageData;
class Stage034Scene : public StageScene
{
public:
    Stage034Scene();
    virtual ~Stage034Scene();
    
public:
    static Stage034Scene* createWithStageData(const StageData*);
    
public:
    virtual RoundBase* loadRound();
    virtual bool isPopShareNode(const unsigned int roundIndex){return roundIndex == 5;};
};

#endif /* defined(__superBaby__Stage034Scene__) */
