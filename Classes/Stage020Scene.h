//
//  Stage020Scene.h
//  AbelLLExplorer
//
//  Created by Administrator on 15/5/20.
//
//

#ifndef __AbelLLExplorer__Stage020Scene__
#define __AbelLLExplorer__Stage020Scene__

#include "StageScene.h"

class Actor;
class StageData;
class Stage020Scene : public StageScene
{
public:
    Stage020Scene();
    virtual ~Stage020Scene();
    
public:
    static Stage020Scene* createWithStageData(const StageData*);
    
public:
    virtual RoundBase* loadRound();
    virtual bool isPopShareNode(const unsigned int roundIndex){return roundIndex == 3;};
};

#endif /* defined(__AbelLLExplorer__Stage020Scene__) */
