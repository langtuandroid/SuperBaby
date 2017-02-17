//
//  Stage029Scene.h
//  AbelLLExplorer
//
//  Created by Administrator on 15/6/4.
//
//

#ifndef __AbelLLExplorer__Stage029Scene__
#define __AbelLLExplorer__Stage029Scene__

#include "StageScene.h"

class Actor;
class StageData;
class Stage029Scene : public StageScene
{
public:
    Stage029Scene();
    virtual ~Stage029Scene();
    
public:
    static Stage029Scene* createWithStageData(const StageData*);
    
public:
    virtual RoundBase* loadRound();
    virtual bool isPopShareNode(const unsigned int roundIndex){return roundIndex == 3;};
};

#endif /* defined(__AbelLLExplorer__Stage029Scene__) */
