//
//  Stage043Scene.h
//  superBaby
//
//  Created by Administrator on 15/7/31.
//
//

#ifndef __superBaby__Stage043Scene__
#define __superBaby__Stage043Scene__

#include "StageScene.h"

class Actor;
class StageData;
class Stage043Scene : public StageScene
{
public:
    Stage043Scene();
    virtual ~Stage043Scene();
    
public:
    static Stage043Scene* createWithStageData(const StageData*);
    
public:
    virtual RoundBase* loadRound();
    virtual bool isPopShareNode(const unsigned int roundIndex){return roundIndex == 5;};
};


#endif /* defined(__superBaby__Stage043Scene__) */
