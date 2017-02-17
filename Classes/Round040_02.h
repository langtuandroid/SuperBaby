//
//  Round040_02.h
//  superBaby
//
//  Created by Administrator on 15/7/24.
//
//

#ifndef __superBaby__Round040_02__
#define __superBaby__Round040_02__

#include "RoundBase.h"

USING_NS_CC;

class Actor;
class StageScene;
class Round040_02 : public RoundBase
{
public:
    Round040_02();
    virtual ~Round040_02();
    
public:
    bool initWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    static Round040_02* createWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    
public:
    void cleanUp();
    virtual void onEnterRound();
    virtual void celebrate(const float);
    /* 开启触摸 */
    virtual void openTouch();
    /*  回合结束回调 */
    virtual void endRound();
    //改变答案的大小
    void scaleChangeAnswersSchedule(float);
    
private:
    Sprite* m_pAnswers[3];
    Vec2 m_answersPos[3];
    int m_effectId = 0;                //音效play的id
    int m_InitAniIndex = 0;
    bool m_opentouched = false;
    bool m_jumped = false;
};


#endif /* defined(__superBaby__Round040_02__) */
