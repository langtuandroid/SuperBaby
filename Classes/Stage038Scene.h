//
//  Stage038Scene.h
//  superBaby
//
//  Created by Administrator on 15/7/21.
//
//

#ifndef __superBaby__Stage038Scene__
#define __superBaby__Stage038Scene__

#include "StageScene.h"

class Actor;
class StageData;
class Stage038Scene : public StageScene
{
public:
    Stage038Scene();
    virtual ~Stage038Scene();
    
public:
    static Stage038Scene* createWithStageData(const StageData*);
    
public:
    virtual RoundBase* loadRound();
    virtual bool isPopShareNode(const unsigned int roundIndex){return roundIndex == 4;};
};

#endif /* defined(__superBaby__Stage038Scene__) */
