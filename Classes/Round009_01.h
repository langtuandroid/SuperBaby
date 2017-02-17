//
//  Round009_01.h
//  AbelLLExplorer
//
//  Created by Administrator on 2015/4/3.
//
//

#ifndef __AbelLLExplorer__Round009_01__
#define __AbelLLExplorer__Round009_01__

#include "RoundBase.h"
USING_NS_CC;

class Actor;
class StageScene;
class Round009_01 : public RoundBase
{
public:
    Round009_01();
    virtual ~Round009_01();
    
public:
    bool initWithRoundIndex(const int,StageScene*, const Vec2& pos = Vec2::ZERO);
    static Round009_01* createWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    
public:
    virtual void onEnterRound();
    virtual void celebrate(const float);
    virtual void onPreLoadedRes();
    virtual void openTouch();
    /*  回合结束回调 */
    virtual void endRound();

private:
    // 是第几只鸟的正确答案
    unsigned int m_correctBirdIndex;
    
    Actor* m_pBirdActor[5];
    Vec2 m_birdPos[5];
    Rect m_targerRect[5];
    int m_effectId;
    Sprite* m_pLeftBackground;  //左边背景图  
};

#endif /* defined(__AbelLLExplorer__Round009_01__) */
