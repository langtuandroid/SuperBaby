//
//  Round038_02.h
//  superBaby
//
//  Created by Administrator on 15/7/21.
//
//

#ifndef __superBaby__Round038_02__
#define __superBaby__Round038_02__

#include "RoundBase.h"

USING_NS_CC;

class Actor;
class StageScene;
class Round038_02 : public RoundBase
{
public:
    Round038_02();
    virtual ~Round038_02();
    
public:
    bool initWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    static Round038_02* createWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    
public:
    void cleanUp();
    virtual void onEnterRound();
    virtual void celebrate(const float);
    /* 开启触摸 */
    virtual void openTouch();
    /*  回合结束回调 */
    virtual void endRound();
    //改变蝙蝠的大小
    void scaleChangeBatsSchedule(float);
    
    //答案上升
    void upAnswersSchedule(float);
    
    //答案下降
    void downAnswersSchedule(float);
    
private:
    Sprite* m_pAnswers[3];
    Vec2 m_answersPos[3];
    Actor* m_pBatsActor[5];
    Vec2 m_batsPos[5];
    int m_effectId = 0;                //音效play的id
    int m_InitAniIndex = 0;
    bool m_opentouched = false;
    bool m_jumped = false;
};

#endif /* defined(__superBaby__Round038_02__) */
