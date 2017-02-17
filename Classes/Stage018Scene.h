//
//  Stage018Scene.h
//  AbelLLExplorer
//
//  Created by Administrator on 15/5/18.
//
//

#ifndef __AbelLLExplorer__Stage018Scene__
#define __AbelLLExplorer__Stage018Scene__

#include "StageScene.h"

class Actor;
class StageData;
class Stage018Scene : public StageScene
{
public:
    Stage018Scene();
    virtual ~Stage018Scene();
    
public:
    static Stage018Scene* createWithStageData(const StageData*);
    
public:
    virtual RoundBase* loadRound();
    virtual bool isPopShareNode(const unsigned int roundIndex){return roundIndex == 4;};
};

#endif /* defined(__AbelLLExplorer__Stage018Scene__) */
