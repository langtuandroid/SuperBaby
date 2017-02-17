//
//  Round041_03.h
//  superBaby
//
//  Created by Administrator on 15/7/28.
//
//

#ifndef __superBaby__Round041_03__
#define __superBaby__Round041_03__

#include "RoundBase.h"

USING_NS_CC;

class Actor;
class StageScene;
class Round041_03 : public RoundBase
{
public:
    Round041_03();
    virtual ~Round041_03();
    
public:
    bool initWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    static Round041_03* createWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    
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
    
    void changeActor();
    
private:
    Sprite* m_pAnswers[5];
    Vec2 m_answersPos[5];
    Vec2 m_answersEndPos[5];
    Actor* m_pMSActor = nullptr;
    Sprite* m_pNestSp = nullptr;
    Sprite* m_pNestOverSp = nullptr;
    int m_effectId = 0;                //音效play的id
    Vec2 m_originalPoint;
    Vec2 m_touchBeganPoint;         // 拖放开始位置
    int m_InitAniIndex = 0;
    bool m_opentouched = false;
    bool m_jumped = false;
};

#endif /* defined(__superBaby__Round041_03__) */
