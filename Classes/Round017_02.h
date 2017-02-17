//
//  Round017_02.h
//  AbelLLExplorer
//
//  Created by Administrator on 15/5/18.
//
//

#ifndef __AbelLLExplorer__Round017_02__
#define __AbelLLExplorer__Round017_02__

#include "RoundBase.h"

USING_NS_CC;

class Actor;
class StageScene;
class Round017_02 : public RoundBase
{
public:
    Round017_02();
    virtual ~Round017_02();
    
public:
    bool initWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    static Round017_02* createWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);

public:
    virtual void onEnterRound();
    virtual void celebrate(const float);
    /* 开启触摸 */
    virtual void openTouch();
    /*  回合结束回调 */
    virtual void endRound();
    
    /*  处理npc收集 */
    virtual void processNpcCollect();
    
    //改变大小出现
    void changeScaleNumbersSchedule(float);
    
    //答对后改变actor动作
    void changeActor();
    
    //答对后隐藏答案
    void scaleChangeNumbers();
    
    //上升飞船
    void upAirShip();
    
private:    
    Sprite* m_pAnswerNumbersSp[3];
    Vec2 m_answerNumbersPos[3];
    Actor* m_pAirShipActor = nullptr;
    
    Actor* m_pNpcActor = nullptr;
    
    unsigned int m_InitAniIndex = 0;
    int m_effectId = 0;
    bool m_opentouched = false;
    bool m_jumped = false;
};

#endif /* defined(__AbelLLExplorer__Round017_02__) */
