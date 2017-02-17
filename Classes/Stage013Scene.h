//
//  Stage013Scene.h
//  AbelLLExplorer
//
//  Created by adinnet on 2015/3/5.
//
//
//

#ifndef __AbelLLExplorer__Stage013Scene__
#define __AbelLLExplorer__Stage013Scene__

#include "StageScene.h"

class Actor;
class StageData;
class Stage013Scene : public StageScene
{
public:
    Stage013Scene();
    virtual ~Stage013Scene();

public:
    static Stage013Scene* createWithStageData(const StageData*);

public:
    virtual RoundBase* loadRound();
    virtual bool isPopShareNode(const unsigned int roundIndex){return roundIndex == 3;};
};

#endif /* defined(__AbelLLExplorer__Stage013Scene__) */
