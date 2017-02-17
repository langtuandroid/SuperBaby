//
//  Stage031Scene.h
//  AbelLLExplorer
//
//  Created by Administrator on 15/6/9.
//
//

#ifndef __AbelLLExplorer__Stage031Scene__
#define __AbelLLExplorer__Stage031Scene__

#include "StageScene.h"

class Actor;
class StageData;
class Stage031Scene : public StageScene
{
public:
    Stage031Scene();
    virtual ~Stage031Scene();
    
public:
    static Stage031Scene* createWithStageData(const StageData*);
    
public:
    virtual RoundBase* loadRound();
    virtual bool isPopShareNode(const unsigned int roundIndex){return roundIndex == 4;};
};

#endif /* defined(__AbelLLExplorer__Stage031Scene__) */
