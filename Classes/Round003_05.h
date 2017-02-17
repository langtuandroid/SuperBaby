//
//  Round003_05.h
//  AbelLLExplorer
//
//  Created by adinnet on 2015/3/6.
//
//  第 3 关卡第 5 回合
//

#ifndef __AbelLLExplorer__Round003_05__
#define __AbelLLExplorer__Round003_05__

#include "RoundBase.h"

USING_NS_CC;

class Actor;
class StageScene;
class Round003_05 : public RoundBase
{
public:
    Round003_05();
    virtual ~Round003_05();
    
public:
    bool initWithRoundIndex(const int,  StageScene*, const Vec2& pos = Vec2::ZERO);
    static Round003_05* createWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
public:
    virtual void onEnterRound();
    virtual void celebrate(const float);
    virtual void onPreLoadedRes();
    /* 开启触摸 */
    virtual void openTouch();
    /*  回合结束回调 */
    virtual void endRound();
    
    //
    void changeScaleSchedule(float);
private:
    
    Sprite* m_pClotheSps[3];            // 衣服
    Actor* m_pActor;                    // 松鼠
    int m_effectId;
    int m_InitAniIndex = 0;
    bool m_opentouched = false;
    bool m_jumped = false;
};

#endif /* defined(__AbelLLExplorer__Round003_05__) */
