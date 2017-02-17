//
//  Stage025Scene.h
//  AbelLLExplorer
//
//  Created by Administrator on 15/5/6.
//
//

#ifndef __AbelLLExplorer__Stage025Scene__
#define __AbelLLExplorer__Stage025Scene__

#include "StageScene.h"

class Actor;
class StageData;
class Stage025Scene : public StageScene
{
public:
    Stage025Scene();
    virtual ~Stage025Scene();
    
public:
    static Stage025Scene* createWithStageData(const StageData*);
    
public:
    virtual RoundBase* loadRound();
    virtual bool isPopShareNode(const unsigned int roundIndex){return roundIndex == 5;};
};

#endif /* defined(__AbelLLExplorer__Stage025Scene__) */
