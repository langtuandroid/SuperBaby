//
//  Round042_03.h
//  superBaby
//
//  Created by Administrator on 15/7/30.
//
//

#ifndef __superBaby__Round042_03__
#define __superBaby__Round042_03__

#include "RoundBase.h"

USING_NS_CC;

class Actor;
class StageScene;
class Round042_03 : public RoundBase
{
public:
    Round042_03();
    virtual ~Round042_03();
    
public:
    bool initWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    static Round042_03* createWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    
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
    
    void changeScaleTopic();
    
    void downAnswersSchedule(float);
    
private:
    Sprite* m_pAnswers[3];
    Vec2 m_answersPos[3];
    Vec2 m_answersEndPos[3];
    Sprite* m_pTopicSp = nullptr;
    Sprite* m_pHintSp = nullptr;
    int m_effectId = 0;                //音效play的id
    Vec2 m_originalPoint;
    Vec2 m_touchBeganPoint;         // 拖放开始位置
    int m_InitAniIndex = 0;
    bool m_opentouched = false;
    bool m_jumped = false;
};


#endif /* defined(__superBaby__Round042_03__) */
