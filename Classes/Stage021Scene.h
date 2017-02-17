//
//  Stage021Scene.h
//  AbelLLExplorer
//
//  Created by Administrator on 15/5/21.
//
//

#ifndef __AbelLLExplorer__Stage021Scene__
#define __AbelLLExplorer__Stage021Scene__

#include "StageScene.h"

class Actor;
class StageData;
class Stage021Scene : public StageScene
{
public:
    Stage021Scene();
    virtual ~Stage021Scene();
    
public:
    static Stage021Scene* createWithStageData(const StageData*);
    
public:
    virtual RoundBase* loadRound();
    virtual bool isPopShareNode(const unsigned int roundIndex){return roundIndex == 4;};
};

#endif /* defined(__AbelLLExplorer__Stage021Scene__) */
