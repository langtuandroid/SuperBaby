//
//  Stage007Scene.h
//  AbelLLExplorer
//
//  Created by Administrator on 2015/4/3.
//
//

#ifndef __AbelLLExplorer__Stage009Scene__
#define __AbelLLExplorer__Stage009Scene__

#include "StageScene.h"

class StageData;
class Stage009Scene :public StageScene
{
public:
    Stage009Scene();
    virtual ~Stage009Scene();
    
public:
    static Stage009Scene* createWithStageData(const StageData*);

public:
    virtual RoundBase* loadRound();
    virtual bool isPopShareNode(const unsigned int roundIndex){return roundIndex == 3;};
};

#endif /* defined(__AbelLLExplorer__Stage009Scene__) */
