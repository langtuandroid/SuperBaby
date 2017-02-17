//
//  Stage014Scene.h
//  AbelLLExplorer
//
//  Created by Administrator on 2015/5/12.
//
//

#ifndef __AbelLLExplorer__Stage014Scene__
#define __AbelLLExplorer__Stage014Scene__

#include "StageScene.h"

class Actor;
class StageData;
class Stage014Scene : public StageScene
{
public:
    Stage014Scene();
    virtual ~Stage014Scene();
    
public:
    static Stage014Scene* createWithStageData(const StageData*);
    
public:
    virtual RoundBase* loadRound();
    virtual bool isPopShareNode(const unsigned int roundIndex){return roundIndex == 4;};
};

#endif /* defined(__AbelLLExplorer__Stage014Scene__) */
