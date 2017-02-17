//
//  Stage007Scene.h
//  AbelLLExplorer
//
//  Created by Administrator on 15/4/3.
//
//

#ifndef __AbelLLExplorer__Stage007Scene__
#define __AbelLLExplorer__Stage007Scene__

#include "StageScene.h"

class StageData;
class Stage007Scene :public StageScene
{
public:
    Stage007Scene();
    virtual ~Stage007Scene();
    
public:
    static Stage007Scene* createWithStageData(const StageData*);
public:
    virtual RoundBase* loadRound();
    virtual bool isPopShareNode(const unsigned int roundIndex){return roundIndex == 4;};
};

#endif /* defined(__AbelLLExplorer__Stage007Scene__) */
