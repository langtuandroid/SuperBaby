//
//  Round043_02.h
//  superBaby
//
//  Created by Administrator on 15/7/31.
//
//

#ifndef __superBaby__Round043_02__
#define __superBaby__Round043_02__


#include "RoundBase.h"

USING_NS_CC;

class Actor;
class StageScene;
class Round043_02 : public RoundBase
{
public:
    Round043_02();
    virtual ~Round043_02();
    
public:
    bool initWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    static Round043_02* createWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    
public:
    void cleanUp();
    virtual void onEnterRound();
    virtual void celebrate(const float);
    /* 开启触摸 */
    virtual void openTouch();
    /*  回合结束回调 */
    virtual void endRound();
    //上升答案
    void upAnswersSchedule(float);
    
    void showNumber();
    
    void downAnswersSchedule(float);
    
private:
    Sprite* m_pAnswers[3];
    Vec2 m_answersPos[3];
    Sprite* m_pBoardSp = nullptr;
    Sprite* m_pNumberSp = nullptr;
    int m_effectId = 0;                //音效play的id
    int m_InitAniIndex = 0;
    bool m_opentouched = false;
    bool m_jumped = false;
};


#endif /* defined(__superBaby__Round043_02__) */
