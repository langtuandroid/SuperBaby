//
//  Stage022Scene.h
//  AbelLLExplorer
//
//  Created by Administrator on 15/5/22.
//
//

#ifndef __AbelLLExplorer__Stage022Scene__
#define __AbelLLExplorer__Stage022Scene__

#include "StageScene.h"

class Actor;
class StageData;
class Stage022Scene : public StageScene
{
public:
    Stage022Scene();
    virtual ~Stage022Scene();
    
public:
    static Stage022Scene* createWithStageData(const StageData*);
    
public:
    virtual RoundBase* loadRound();
    virtual bool isPopShareNode(const unsigned int roundIndex){return roundIndex == 3;};
};


#endif /* defined(__AbelLLExplorer__Stage022Scene__) */
