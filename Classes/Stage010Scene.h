//
//  Stage010Scene.h
//  AbelLLExplorer
//
//  Created by Administrator on 2015/4/8.
//
//

#ifndef __AbelLLExplorer__Stage010Scene__
#define __AbelLLExplorer__Stage010Scene__

#include "StageScene.h"

class StageData;
class Stage010Scene :public StageScene
{
public:
    Stage010Scene();
    virtual ~Stage010Scene();
    
public:
    static Stage010Scene* createWithStageData(const StageData*);
public:
    virtual RoundBase* loadRound();
    virtual bool isPopShareNode(const unsigned int roundIndex){return roundIndex == 3;};
};

#endif /* defined(__AbelLLExplorer__Stage010Scene__) */
