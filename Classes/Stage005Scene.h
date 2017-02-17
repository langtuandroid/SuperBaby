//
//  Stage005Scene.h
//  AbelLLExplorer
//
//  Created by Administrator on 15/4/1.
//
//

#ifndef __AbelLLExplorer__Stage005Scene__
#define __AbelLLExplorer__Stage005Scene__

#include "StageScene.h"

class StageData;
class Stage005Scene :public StageScene
{
public:
    Stage005Scene();
    virtual ~Stage005Scene();
    
public:
    static Stage005Scene* createWithStageData(const StageData*);
public:
    virtual RoundBase* loadRound();
    virtual bool isPopShareNode(const unsigned int roundIndex){return roundIndex == 5;};
};

#endif /* defined(__AbelLLExplorer__Stage005Scene__) */
