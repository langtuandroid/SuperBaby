//
//  Stage004Scene.h
//  AbelLLExplorer
//
//  Created by Administrator on 15/3/27.
//
//

#ifndef __AbelLLExplorer__Stage004Scene__
#define __AbelLLExplorer__Stage004Scene__

#include "StageScene.h"

class StageData;
class Stage004Scene :public StageScene
{
public:
    Stage004Scene();
    virtual ~Stage004Scene();
    
public:
    static Stage004Scene* createWithStageData(const StageData*);
public:
    virtual RoundBase* loadRound();
    virtual bool isPopShareNode(const unsigned int roundIndex){return roundIndex == 4;};
};

#endif /* defined(__AbelLLExplorer__Stage004Scene__) */
