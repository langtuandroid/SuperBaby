//
//  Round039_02.h
//  superBaby
//
//  Created by Administrator on 15/7/23.
//
//

#ifndef __superBaby__Round039_02__
#define __superBaby__Round039_02__

#include "RoundBase.h"

USING_NS_CC;

class Actor;
class StageScene;
class Round039_02 : public RoundBase
{
public:
    Round039_02();
    virtual ~Round039_02();
    
public:
    bool initWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    static Round039_02* createWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    
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
    
    void fadeOutAll();
    
    void moveLeadActor();
    
private:
    Sprite* m_pAnswers[4];
    Vec2 m_answersPos[4];
    int m_effectId = 0;                //音效play的id
    int m_InitAniIndex = 0;
    bool m_opentouched = false;
    bool m_jumped = false;
    Sprite* m_pMonkeySp = nullptr;
    Sprite* m_pStageSp = nullptr;
};


#endif /* defined(__superBaby__Round039_02__) */
