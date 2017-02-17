//
//  Stage006Scene.h
//  AbelLLExplorer
//
//  Created by Administrator on 2015/4/2.
//
//

#ifndef __AbelLLExplorer__Stage006Scene__
#define __AbelLLExplorer__Stage006Scene__

#include "StageScene.h"

class StageData;
class Stage006Scene :public StageScene
{
public:
    Stage006Scene();
    virtual ~Stage006Scene();
    
public:
    static Stage006Scene* createWithStageData(const StageData*);
public:
    virtual RoundBase* loadRound();
    virtual bool isPopShareNode(const unsigned int roundIndex){return roundIndex == 5;};
};

#endif /* defined(__AbelLLExplorer__Stage006Scene__) */
