//
//  Stage017Scene.h
//  AbelLLExplorer
//
//  Created by Administrator on 15/5/18.
//
//

#ifndef __AbelLLExplorer__Stage017Scene__
#define __AbelLLExplorer__Stage017Scene__

#include "StageScene.h"

class Actor;
class StageData;
class Stage017Scene : public StageScene
{
public:
    Stage017Scene();
    virtual ~Stage017Scene();
    
public:
    static Stage017Scene* createWithStageData(const StageData*);
    
public:
    virtual RoundBase* loadRound();
    virtual bool isPopShareNode(const unsigned int roundIndex){return roundIndex == 3;};
};

#endif /* defined(__AbelLLExplorer__Stage017Scene__) */
