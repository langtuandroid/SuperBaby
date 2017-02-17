//
//  Round024_03.h
//  AbelLLExplorer
//
//  Created by adinnet on 2015/3/9.
//
//

#ifndef __AbelLLExplorer__Round024_03__
#define __AbelLLExplorer__Round024_03__

#include "RoundBase.h"

USING_NS_CC;

class Actor;
class LeadingActor;
class StageScene;
class Round024_03 : public RoundBase
{
public:
    Round024_03();
    virtual ~Round024_03();
    
public:
    bool initWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    static Round024_03* createWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    
public:
    virtual void onEnterRound();
    virtual void celebrate(const float);
    virtual void onPreLoadedRes();
    /* 开启触摸 */
    virtual void openTouch();
    /*  回合结束回调 */
    virtual void endRound();
    
private:
    Actor* m_pRobotActor = nullptr;
    
    Rect m_robot1Rect[3];
    Rect m_robot2Rect[3];
    bool m_answered[3] = {false, false, false};
    Vec2 m_robot1GouxuanPos[3];
    Vec2 m_robot2GouxuanPos[3];
    
    Actor* m_pGouActor = nullptr;
    std::vector<Actor*> m_pGouxuanActors;
    
    int m_effectId = 0;                //音效play的id
};

#endif /* defined(__AbelLLExplorer__Round024_03__) */
