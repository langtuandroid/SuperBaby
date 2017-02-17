//
//  Stage032Scene.h
//  AbelLLExplorer
//
//  Created by Administrator on 15/6/9.
//
//

#ifndef __AbelLLExplorer__Stage032Scene__
#define __AbelLLExplorer__Stage032Scene__

#include "StageScene.h"

class Actor;
class StageData;
class Stage032Scene : public StageScene
{
public:
    Stage032Scene();
    virtual ~Stage032Scene();
    
public:
    static Stage032Scene* createWithStageData(const StageData*);
    
public:
    virtual RoundBase* loadRound();
    virtual bool isPopShareNode(const unsigned int roundIndex){return roundIndex == 4;};
};

#endif /* defined(__AbelLLExplorer__Stage032Scene__) */
