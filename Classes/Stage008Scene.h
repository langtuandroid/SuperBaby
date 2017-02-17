//
//  Stage008Scene.h
//  AbelLLExplorer
//
//  Created by Administrator on 2015/4/7.
//
//

#ifndef __AbelLLExplorer__Stage008Scene__
#define __AbelLLExplorer__Stage008Scene__

#include "StageScene.h"

class StageData;
class Stage008Scene :public StageScene
{
public:
    Stage008Scene();
    virtual ~Stage008Scene();
    
public:
    static Stage008Scene* createWithStageData(const StageData*);
public:
    virtual RoundBase* loadRound();
    virtual bool isPopShareNode(const unsigned int roundIndex){return roundIndex == 3;};
};

#endif /* defined(__AbelLLExplorer__Stage008Scene__) */
