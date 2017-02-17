//
//  Stage023Scene.h
//  AbelLLExplorer
//
//  Created by adinnet on 2015/3/5.
//
//
//

#ifndef __AbelLLExplorer__Stage023Scene__
#define __AbelLLExplorer__Stage023Scene__

#include "StageScene.h"

class Actor;
class StageData;
class Stage023Scene : public StageScene
{
public:
    Stage023Scene();
    virtual ~Stage023Scene();

public:
    static Stage023Scene* createWithStageData(const StageData*);

public:
    virtual RoundBase* loadRound();
    virtual bool isPopShareNode(const unsigned int roundIndex){return roundIndex == 3;};
};

#endif /* defined(__AbelLLExplorer__Stage023Scene__) */
