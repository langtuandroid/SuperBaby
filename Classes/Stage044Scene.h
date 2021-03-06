//
//  Stage044Scene.h
//  superBaby
//
//  Created by Administrator on 15/8/3.
//
//

#ifndef __superBaby__Stage044Scene__
#define __superBaby__Stage044Scene__

#include "StageScene.h"

class Actor;
class StageData;
class Stage044Scene : public StageScene
{
public:
    Stage044Scene();
    virtual ~Stage044Scene();
    
public:
    static Stage044Scene* createWithStageData(const StageData*);
    
public:
    virtual RoundBase* loadRound();
    virtual bool isPopShareNode(const unsigned int roundIndex){return roundIndex == 5;};
};


#endif /* defined(__superBaby__Stage044Scene__) */
