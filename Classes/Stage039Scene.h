//
//  Stage039Scene.h
//  superBaby
//
//  Created by Administrator on 15/7/23.
//
//

#ifndef __superBaby__Stage039Scene__
#define __superBaby__Stage039Scene__

#include "StageScene.h"

class Actor;
class StageData;
class Stage039Scene : public StageScene
{
public:
    Stage039Scene();
    virtual ~Stage039Scene();
    
public:
    static Stage039Scene* createWithStageData(const StageData*);
    
public:
    virtual RoundBase* loadRound();
    virtual bool isPopShareNode(const unsigned int roundIndex){return roundIndex == 4;};
};

#endif /* defined(__superBaby__Stage039Scene__) */
