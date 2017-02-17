//
//  Stage016Scene.h
//  AbelLLExplorer
//
//  Created by Administrator on 15/5/15.
//
//

#ifndef __AbelLLExplorer__Stage016Scene__
#define __AbelLLExplorer__Stage016Scene__

#include "StageScene.h"

class Actor;
class StageData;
class Stage016Scene : public StageScene
{
public:
    Stage016Scene();
    virtual ~Stage016Scene();
    
public:
    static Stage016Scene* createWithStageData(const StageData*);
    
public:
    virtual RoundBase* loadRound();
    virtual bool isPopShareNode(const unsigned int roundIndex){return roundIndex == 4;};
};

#endif /* defined(__AbelLLExplorer__Stage016Scene__) */
