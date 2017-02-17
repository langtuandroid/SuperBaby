//
//  Stage011Scene.h
//  AbelLLExplorer
//
//  Created by Administrator on 2015/4/9.
//
//

#ifndef __AbelLLExplorer__Stage011Scene__
#define __AbelLLExplorer__Stage011Scene__

#include "StageScene.h"

class StageData;
class Stage011Scene :public StageScene
{
public:
    Stage011Scene();
    virtual ~Stage011Scene();
    
public:
    static Stage011Scene* createWithStageData(const StageData*);

public:
    virtual RoundBase* loadRound();
    virtual bool isPopShareNode(const unsigned int roundIndex){return roundIndex == 5;};
};

#endif /* defined(__AbelLLExplorer__Stage011Scene__) */
