//
//  Round003_02.h
//  AbelLLExplorer
//
//  Created by adinnet on 2015/3/6.
//
//  第 3 关卡第 2 回合
//

#ifndef __AbelLLExplorer__Round003_02__
#define __AbelLLExplorer__Round003_02__

#include "RoundBase.h"
#include <vector>

USING_NS_CC;

class Actor;
class StageScene;
class Round003_02 : public RoundBase
{
public:
    Round003_02();
    virtual ~Round003_02();

public:
    bool initWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    static Round003_02* createWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    
public:
    virtual void onEnterRound();
    virtual void celebrate(const float);
    virtual void onPreLoadedRes();
    /* 开启触摸 */
    virtual void openTouch();
    /*  回合结束回调 */
    virtual void endRound();
    
    void birdFlyedComplete();
    void birdFlySchedule(float);

private:
    Actor* m_pBird1Actor = nullptr;
    Actor* m_pBird2Actor = nullptr;
    
    Rect m_bird1Rect[3];
    Rect m_bird2Rect[3];
    bool m_answered[3];
    Vec2 m_bird1GouxuanPos[3];
    Vec2 m_bird2GouxuanPos[3];
    
    Rect m_touchArea;
    
    Actor* m_pGouActor = nullptr;
    std::vector<Actor*> m_pGouxuanActors;
    int m_effectId = 0;
};

#endif /* defined(__AbelLLExplorer__Round003_02__) */
