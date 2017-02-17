//
//  Stage045Scene.h
//  superBaby
//
//  Created by Administrator on 15/8/3.
//
//

#ifndef __superBaby__Stage045Scene__
#define __superBaby__Stage045Scene__

#include "StageScene.h"

class Actor;
class StageData;
class Stage045Scene : public StageScene
{
public:
    Stage045Scene();
    virtual ~Stage045Scene();
    
public:
    static Stage045Scene* createWithStageData(const StageData*);
    
public:
    virtual RoundBase* loadRound();
    virtual bool isPopShareNode(const unsigned int roundIndex){return roundIndex == 3;};
};


#endif /* defined(__superBaby__Stage045Scene__) */
