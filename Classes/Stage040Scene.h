//
//  Stage040Scene.h
//  superBaby
//
//  Created by Administrator on 15/7/24.
//
//

#ifndef __superBaby__Stage040Scene__
#define __superBaby__Stage040Scene__

#include "StageScene.h"

class Actor;
class StageData;
class Stage040Scene : public StageScene
{
public:
    Stage040Scene();
    virtual ~Stage040Scene();
    
public:
    static Stage040Scene* createWithStageData(const StageData*);
    
public:
    virtual RoundBase* loadRound();
    virtual bool isPopShareNode(const unsigned int roundIndex){return roundIndex == 5;};
};

#endif /* defined(__superBaby__Stage040Scene__) */
