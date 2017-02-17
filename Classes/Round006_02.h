//
//  Round006_02.h
//  AbelLLExplorer
//
//  Created by Administrator on 15/4/2.
//
//

#ifndef __AbelLLExplorer__Round006_02__
#define __AbelLLExplorer__Round006_02__

#include "RoundBase.h"
USING_NS_CC;

class Actor;
class StageScene;
class Round006_02 : public RoundBase
{
public:
    Round006_02();
    virtual ~Round006_02();
    
public:
    bool initWithRoundIndex(const int,StageScene*, const Vec2& pos = Vec2::ZERO);
    static Round006_02* createWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    
public:
    virtual void celebrate(const float);
    virtual void onPreLoadedRes();
    /* 开启触摸 */
    virtual void openTouch();
    /*  回合结束回调 */
    virtual void endRound();

    // 回合开场动画，小数字上升动画调度
    void numberUpSchedule(float);
    // 回合结束动画，小数字下降动画调度
    void numberDownSchedule(float);
    
    //答对蝴蝶飞走动画
    void changeButterflyMove();
    
private:
    void cleanUp();
    virtual void onEnterRound();
    
private:
    Sprite* m_pChooseNumber[3];       //选择的数字
    unsigned int m_InitAniIndex;
    Actor* m_pButterflyActor;             //蝴蝶
    int m_effectId;
    bool m_opentouched = false;
    bool m_jumped = false;
};

#endif /* defined(__AbelLLExplorer__Round006_02__) */
