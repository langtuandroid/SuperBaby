//
//  Round045_01.h
//  superBaby
//
//  Created by Administrator on 15/8/3.
//
//

#ifndef __superBaby__Round045_01__
#define __superBaby__Round045_01__

#include "RoundBase.h"
USING_NS_CC;
class Actor;
class StageScene;
class Round045_01 : public RoundBase
{
public:
    Round045_01();
    virtual ~Round045_01();
    
public:
    bool initWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    static Round045_01* createWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    
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
    
    //提示答案
    void changeScaleAnswersSchedule(float);
    
    void changeActor();
    
private:
    Actor* m_pNpcActor = nullptr;
    Sprite* m_pAnswersSp[4];
    Vec2 m_answersPos[4];
    Sprite* m_pShawdowSp = nullptr;
    bool m_opentouched = false;
    bool m_jumped = false;
    unsigned int m_InitAniIndex = 0;
    int m_effectId = 0;

};


#endif /* defined(__superBaby__Round045_01__) */
