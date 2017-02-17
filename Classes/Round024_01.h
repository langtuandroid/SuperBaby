//
//  Round024_01.h
//  AbelLLExplorer
//
//  Created by adinnet on 2015/03/06.
//
//  神秘星球 第24关卡第一回合
//

#ifndef __AbelLLExplorer__Round024_01__
#define __AbelLLExplorer__Round024_01__

#include "RoundBase.h"

USING_NS_CC;

class Actor;
class StageScene;
class Round024_01 : public RoundBase
{
public:
    Round024_01();
    virtual ~Round024_01();

public:
    bool initWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    static Round024_01* createWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    
public:
    virtual void onEnterRound();
    virtual void celebrate(const float);
    /* 开启触摸 */
    virtual void openTouch();
    /*  回合结束回调 */
    virtual void endRound();
    
    void changeScaleRobotsSchedule(float);
    
private:
    // 三个机器人
    Actor* m_robotActors[3];
    
    //
    int m_effectId = 0;                //音效play的id
    
    long m_correctRobotIndex = 2;
    int m_InitAniIndex = 0;
    bool m_opentouched = false;
    bool m_jumped = false;
};


#endif /* defined(__AbelLLExplorer__Round024_01__) */
