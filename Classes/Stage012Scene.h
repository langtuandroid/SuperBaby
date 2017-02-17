//
//  Stage013Scene.h
//  AbelLLExplorer
//
//  Created by Administrator on 2015/4/9.
//
//

#ifndef __AbelLLExplorer__Stage012Scene__
#define __AbelLLExplorer__Stage012Scene__

#include "StageScene.h"

class StageData;
class Stage012Scene :public StageScene
{
public:
    Stage012Scene();
    virtual ~Stage012Scene();
    
public:
    static Stage012Scene* createWithStageData(const StageData*);
public:
    virtual RoundBase* loadRound();
    virtual bool isPopShareNode(const unsigned int roundIndex){return roundIndex == 3;};
};

#endif /* defined(__AbelLLExplorer__Stage011Scene__) */
