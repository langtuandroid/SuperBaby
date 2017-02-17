//
//  Round004_01.h
//  AbelLLExplorer
//
//  Created by Administrator on 15/3/27.
//
//

#ifndef __AbelLLExplorer__Round004_01__
#define __AbelLLExplorer__Round004_01__

#include "RoundBase.h"

USING_NS_CC;

class Actor;
class StageScene;
class Round004_01 : public RoundBase
{
public:
    Round004_01();
    virtual ~Round004_01();
    
public:
    bool initWithRoundIndex(const int,StageScene*, const Vec2& pos = Vec2::ZERO);
    static Round004_01* createWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    
public:
    virtual void celebrate(const float);
    /* 开启触摸 */
    virtual void openTouch();
    /*  回合结束回调 */
    virtual void endRound();
    
    //选择正确时改变npc动作
    void changeNpcAni();
    // 回合开场动画，小数字上升动画调度
    void numberUpSchedule(float);
    // 回合结束动画，小数字下降动画调度
    void numberDownSchedule(float);

private:
    void cleanUp();
    virtual void onEnterRound();
    
private:
    Actor* m_pRabbitNpc;              //npc兔子
    Actor* m_pBag;                    //袋子
    Sprite* m_pChooseNumber[3];       //选择的数字
    unsigned int m_InitAniIndex;
    
    int m_effectId;
    bool m_opentouched = false;
    bool m_jumped = false;
};

#endif /* defined(__AbelLLExplorer__Round004_01__) */
