//
//  Round038_03.h
//  superBaby
//
//  Created by Administrator on 15/7/21.
//
//

#ifndef __superBaby__Round038_03__
#define __superBaby__Round038_03__

#include "RoundBase.h"

USING_NS_CC;

class Actor;
class StageScene;
class Round038_03 : public RoundBase
{
public:
    Round038_03();
    virtual ~Round038_03();
    
public:
    bool initWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    static Round038_03* createWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    
public:
    void cleanUp();
    virtual void onEnterRound();
    virtual void celebrate(const float);
    /* 开启触摸 */
    virtual void openTouch();
    /*  回合结束回调 */
    virtual void endRound();
    
    //放大答案
    void scaleChangeAnswersSchedule(float);
    
    //答对后渐隐答案
    void fadeOutAnswers();
    //移动主角
    void moveHeadActor();
    
private:
    Sprite* m_pAnswersSp[5];
    Vec2 m_answersPos[5];
    int m_effectId = 0;                 //音效play的id
    int m_curSelIndex = -1;              //
    int m_InitAniIndex = 0;
    bool m_opentouched = false;
    bool m_jumped = false;
    unsigned int m_randNum = 0;
    unsigned int m_curCorrectNum = 0;
    Sprite* m_pStageSp = nullptr;
};

#endif /* defined(__superBaby__Round038_03__) */
