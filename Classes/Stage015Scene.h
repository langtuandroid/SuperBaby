//
//  Stage015Scene.h
//  AbelLLExplorer
//
//  Created by Administrator on 15/5/14.
//
//

#ifndef __AbelLLExplorer__Stage015Scene__
#define __AbelLLExplorer__Stage015Scene__

#include "StageScene.h"

class Actor;
class StageData;
class Stage015Scene : public StageScene
{
public:
    Stage015Scene();
    virtual ~Stage015Scene();
    
public:
    static Stage015Scene* createWithStageData(const StageData*);
    
public:
    virtual RoundBase* loadRound();
    virtual bool isPopShareNode(const unsigned int roundIndex){return roundIndex == 3;};
};

#endif /* defined(__AbelLLExplorer__Stage015Scene__) */
