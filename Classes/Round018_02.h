//
//  Round018_02.h
//  AbelLLExplorer
//
//  Created by Administrator on 15/5/18.
//
//

#ifndef __AbelLLExplorer__Round018_02__
#define __AbelLLExplorer__Round018_02__

#include "RoundBase.h"

USING_NS_CC;

class Actor;
class StageScene;
class Round018_02 : public RoundBase
{
public:
    Round018_02();
    virtual ~Round018_02();
    
public:
    bool initWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    static Round018_02* createWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    
public:
    virtual void onEnterRound();
    virtual void celebrate(const float);
    /* 开启触摸 */
    virtual void openTouch();
    /*  回合结束回调 */
    virtual void endRound();
    
    //下降渐变出现
    void downFadeInSchedule(float);
    
    //上升渐变隐藏
    void upFadeOutSchedule(float);
    
private:
    Sprite* m_pBeansSp[5];
    Vec2 m_beansPos[5];
    
    unsigned int m_InitAniIndex = 0;
    int m_effectId = 0;
    bool m_opentouched = false;
    bool m_jumped = false;
};


#endif /* defined(__AbelLLExplorer__Round018_02__) */
