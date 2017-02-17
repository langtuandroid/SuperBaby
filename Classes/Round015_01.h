//
//  Round015_01.h
//  AbelLLExplorer
//
//  Created by Administrator on 15/5/14.
//
//

#ifndef __AbelLLExplorer__Round015_01__
#define __AbelLLExplorer__Round015_01__

#include "RoundBase.h"

USING_NS_CC;

class Actor;
class StageScene;
class Round015_01 : public RoundBase
{
public:
    Round015_01();
    virtual ~Round015_01();
    
public:
    bool initWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    static Round015_01* createWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    
public:

    
public:
    virtual void onEnterRound();
    virtual void celebrate(const float);
    /* 开启触摸 */
    virtual void openTouch();
    /*  回合结束回调 */
    virtual void endRound();
    /*  处理npc收集 */
    virtual void processNpcCollect();
    
    //答对后改变actor动作
    void changeActor();
    
private:    
    Actor* m_pRobotActors[3];
    
    Vec2 m_robotsPos[3];
    
    
    unsigned int m_InitAniIndex = 0;
    int m_effectId = 0;
    
};

#endif /* defined(__AbelLLExplorer__Round015_01__) */
