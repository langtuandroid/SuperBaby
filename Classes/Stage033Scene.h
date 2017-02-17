//
//  Stage033Scene.h
//  AbelLLExplorer
//
//  Created by Administrator on 15/6/10.
//
//

#ifndef __AbelLLExplorer__Stage033Scene__
#define __AbelLLExplorer__Stage033Scene__

#include "StageScene.h"

class Actor;
class StageData;
class Stage033Scene : public StageScene
{
public:
    Stage033Scene();
    virtual ~Stage033Scene();
    
public:
    static Stage033Scene* createWithStageData(const StageData*);
    
public:
    virtual RoundBase* loadRound();
    virtual bool isPopShareNode(const unsigned int roundIndex){return roundIndex == 4;};
};

#endif /* defined(__AbelLLExplorer__Stage033Scene__) */
