//
//  Stage001Scene.h
//  AbelLLExplorer
//
//  Created by adinnet on 2015/3/5.
//
//
//

#ifndef __AbelLLExplorer__Stage001Scene__
#define __AbelLLExplorer__Stage001Scene__

#include "StageScene.h"

class Actor;
class StageData;
class Stage001Scene : public StageScene
{
public:
    Stage001Scene();
    virtual ~Stage001Scene();

public:
    static Stage001Scene* createWithStageData(const StageData*);

public:
    virtual RoundBase* loadRound();
    virtual bool isPopShareNode(const unsigned int roundIndex){return roundIndex == 5;};
};

#endif /* defined(__AbelLLExplorer__Stage001Scene__) */
