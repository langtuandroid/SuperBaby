//
//  Stage046Scene.h
//  superBaby
//
//  Created by Administrator on 15/8/4.
//
//

#ifndef __superBaby__Stage046Scene__
#define __superBaby__Stage046Scene__

#include "StageScene.h"

class Actor;
class StageData;
class Stage046Scene : public StageScene
{
public:
    Stage046Scene();
    virtual ~Stage046Scene();
    
public:
    static Stage046Scene* createWithStageData(const StageData*);
    
public:
    virtual RoundBase* loadRound();
    virtual bool isPopShareNode(const unsigned int roundIndex){return roundIndex == 3;};
};


#endif /* defined(__superBaby__Stage046Scene__) */
