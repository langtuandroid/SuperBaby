//
//  Stage028Scene.h
//  AbelLLExplorer
//
//  Created by Administrator on 15/6/4.
//
//

#ifndef __AbelLLExplorer__Stage028Scene__
#define __AbelLLExplorer__Stage028Scene__

#include "StageScene.h"

class Actor;
class StageData;
class Stage028Scene : public StageScene
{
public:
    Stage028Scene();
    virtual ~Stage028Scene();
    
public:
    static Stage028Scene* createWithStageData(const StageData*);
    
public:
    virtual RoundBase* loadRound();
    virtual bool isPopShareNode(const unsigned int roundIndex){return roundIndex == 4;};
};

#endif /* defined(__AbelLLExplorer__Stage028Scene__) */
