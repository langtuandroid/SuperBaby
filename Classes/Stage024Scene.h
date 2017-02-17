//
//  Stage024Scene.h
//  AbelLLExplorer
//
//  Created by adinnet on 2015/3/5.
//
//
//

#ifndef __AbelLLExplorer__Stage024Scene__
#define __AbelLLExplorer__Stage024Scene__

#include "StageScene.h"

class Actor;
class StageData;
class Stage024Scene : public StageScene
{
public:
    Stage024Scene();
    virtual ~Stage024Scene();

public:
    static Stage024Scene* createWithStageData(const StageData*);

public:
    virtual RoundBase* loadRound();
    virtual bool isPopShareNode(const unsigned int roundIndex){return roundIndex == 4;};
};

#endif /* defined(__AbelLLExplorer__Stage024Scene__) */
