//
//  Stage030Scene.h
//  AbelLLExplorer
//
//  Created by Administrator on 15/6/8.
//
//

#ifndef __AbelLLExplorer__Stage030Scene__
#define __AbelLLExplorer__Stage030Scene__

#include "StageScene.h"

class Actor;
class StageData;
class Stage030Scene : public StageScene
{
public:
    Stage030Scene();
    virtual ~Stage030Scene();
    
public:
    static Stage030Scene* createWithStageData(const StageData*);
    
public:
    virtual RoundBase* loadRound();
    virtual bool isPopShareNode(const unsigned int roundIndex){return roundIndex == 5;};
};

#endif /* defined(__AbelLLExplorer__Stage030Scene__) */
