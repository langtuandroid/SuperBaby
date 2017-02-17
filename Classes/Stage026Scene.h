//
//  Stage026Scene.h
//  AbelLLExplorer
//
//  Created by Administrator on 15/6/1.
//
//

#ifndef __AbelLLExplorer__Stage026Scene__
#define __AbelLLExplorer__Stage026Scene__

#include "StageScene.h"

class Actor;
class StageData;
class Stage026Scene : public StageScene
{
public:
    Stage026Scene();
    virtual ~Stage026Scene();
    
public:
    static Stage026Scene* createWithStageData(const StageData*);
    
public:
    virtual RoundBase* loadRound();
    virtual bool isPopShareNode(const unsigned int roundIndex){return roundIndex == 4;};
};

#endif /* defined(__AbelLLExplorer__Stage026Scene__) */
