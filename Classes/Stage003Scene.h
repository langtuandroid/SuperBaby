//
//  Stage002Scene.h
//  AbelLLExplorer
//
//  Created by adinnet on 2015/3/5.
//
//  第三关
//

#ifndef __AbelLLExplorer__Stage003Scene__
#define __AbelLLExplorer__Stage003Scene__

#include "StageScene.h"

class StageData;
class Stage003Scene : public StageScene
{
public:
    Stage003Scene();
    virtual ~Stage003Scene();

public:
    static Stage003Scene* createWithStageData(const StageData*);

public:
    virtual RoundBase* loadRound();
    virtual bool isPopShareNode(const unsigned int roundIndex){return roundIndex == 5;};
};

#endif /* defined(__AbelLLExplorer__Stage002Scene__) */
