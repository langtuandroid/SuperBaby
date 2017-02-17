//
//  Stage027Scene.h
//  AbelLLExplorer
//
//  Created by Administrator on 15/6/2.
//
//

#ifndef __AbelLLExplorer__Stage027Scene__
#define __AbelLLExplorer__Stage027Scene__

#include "StageScene.h"

class Actor;
class StageData;
class Stage027Scene : public StageScene
{
public:
    Stage027Scene();
    virtual ~Stage027Scene();
    
public:
    static Stage027Scene* createWithStageData(const StageData*);
    
public:
    virtual RoundBase* loadRound();
    virtual bool isPopShareNode(const unsigned int roundIndex){return roundIndex == 4;};
};

#endif /* defined(__AbelLLExplorer__Stage027Scene__) */
