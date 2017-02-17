//
//  Stage037Scene.h
//  AbelLLExplorer
//
//  Created by Administrator on 15/5/8.
//
//

#ifndef __AbelLLExplorer__Stage037Scene__
#define __AbelLLExplorer__Stage037Scene__

#include "StageScene.h"

class Actor;
class StageData;
class Stage037Scene : public StageScene
{
public:
    Stage037Scene();
    virtual ~Stage037Scene();
    
public:
    static Stage037Scene* createWithStageData(const StageData*);
    
public:
    virtual RoundBase* loadRound();
    virtual bool isPopShareNode(const unsigned int roundIndex){return roundIndex == 5;};
};

#endif /* defined(__AbelLLExplorer__Stage037Scene__) */
