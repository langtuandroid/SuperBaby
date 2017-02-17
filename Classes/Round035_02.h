//
//  Round035_02.h
//  superBaby
//
//  Created by Administrator on 15/7/6.
//
//

#ifndef __superBaby__Round035_02__
#define __superBaby__Round035_02__

#include "RoundBase.h"

USING_NS_CC;

class Actor;
class StageScene;
class Round035_02 : public RoundBase
{
public:
    Round035_02();
    virtual ~Round035_02();
    
public:
    bool initWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    static Round035_02* createWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    
public:
    void cleanUp();
    virtual void onEnterRound();
    virtual void celebrate(const float);
    /* 开启触摸 */
    virtual void openTouch();
    /*  回合结束回调 */
    virtual void endRound();
    
    //依次放大答案以提示
    void changeScaleAnswersSchedule(float);
    //答对后移动鱼
    void moveFishes();
private:
    Actor* m_pAnswersActor[3];
    Vec2 m_answersPos[3];
    int m_InitAniIndex = 0;
    int m_effectId = 0;                //音效play的id
    bool m_opentouched = false;
    bool m_jumped = false;
    Actor* m_pPaopaoActor = nullptr;
    Actor* m_pWhiteLightActor = nullptr;
};

#endif /* defined(__superBaby__Round035_02__) */
