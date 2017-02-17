//
//  Stage048Scene.h
//  superBaby
//
//  Created by Administrator on 15/8/6.
//
//

#ifndef __superBaby__Stage048Scene__
#define __superBaby__Stage048Scene__

#include "StageScene.h"

class Actor;
class StageData;
class Stage048Scene : public StageScene
{
public:
    Stage048Scene();
    virtual ~Stage048Scene();
    
public:
    static Stage048Scene* createWithStageData(const StageData*);
    
public:
    virtual RoundBase* loadRound();
    virtual bool isPopShareNode(const unsigned int roundIndex){return roundIndex == 5;};
};

#endif /* defined(__superBaby__Stage048Scene__) */
