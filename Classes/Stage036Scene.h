//
//  Stage036Scene.h
//  superBaby
//
//  Created by Administrator on 15/7/7.
//
//

#ifndef __superBaby__Stage036Scene__
#define __superBaby__Stage036Scene__

#include "StageScene.h"

class Actor;
class StageData;
class Stage036Scene : public StageScene
{
public:
    Stage036Scene();
    virtual ~Stage036Scene();
    
public:
    static Stage036Scene* createWithStageData(const StageData*);
    
public:
    virtual RoundBase* loadRound();
    virtual bool isPopShareNode(const unsigned int roundIndex){return roundIndex == 4;};
};

#endif /* defined(__superBaby__Stage036Scene__) */
