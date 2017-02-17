//
//  Stage002Scene.h
//  AbelLLExplorer
//
//  Created by adinnet on 2015/3/5.
//
//  第一关
//

#ifndef __AbelLLExplorer__Stage002Scene__
#define __AbelLLExplorer__Stage002Scene__

#include "StageScene.h"

class StageData;
class Stage002Scene : public StageScene
{
public:
    Stage002Scene();
    virtual ~Stage002Scene();

public:
    static Stage002Scene* createWithStageData(const StageData*);

public:
    virtual RoundBase* loadRound();
    virtual bool isPopShareNode(const unsigned int roundIndex){return roundIndex == 3;};
};

#endif /* defined(__AbelLLExplorer__Stage002Scene__) */
