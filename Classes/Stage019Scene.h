//
//  Stage019Scene.h
//  AbelLLExplorer
//
//  Created by Administrator on 15/5/19.
//
//

#ifndef __AbelLLExplorer__Stage019Scene__
#define __AbelLLExplorer__Stage019Scene__

#include "StageScene.h"

class Actor;
class StageData;
class Stage019Scene : public StageScene
{
public:
    Stage019Scene();
    virtual ~Stage019Scene();
    
public:
    static Stage019Scene* createWithStageData(const StageData*);
    
public:
    virtual RoundBase* loadRound();
    virtual bool isPopShareNode(const unsigned int roundIndex){return roundIndex == 3;};
};

#endif /* defined(__AbelLLExplorer__Stage019Scene__) */
