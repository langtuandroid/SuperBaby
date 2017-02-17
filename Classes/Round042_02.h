//
//  Round042_02.h
//  superBaby
//
//  Created by Administrator on 15/7/30.
//
//

#ifndef __superBaby__Round042_02__
#define __superBaby__Round042_02__

#include "RoundBase.h"
USING_NS_CC;

class Actor;
class StageScene;
class Round042_02 : public RoundBase
{
public:
    Round042_02();
    virtual ~Round042_02();
    
public:
    bool initWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    static Round042_02* createWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    
public:
    void cleanUp();
    virtual void onEnterRound();
    virtual void celebrate(const float);
    /* 开启触摸 */
    virtual void openTouch();
    /*  回合结束回调 */
    virtual void endRound();
    
    //上升答案以显示
    void upAnswersSchedule(float);
    
    void changeScaleHint();
    
    void fadeOutAnswers();
        
private:
    Sprite* m_pAnswersSp[3];
    Vec2 m_answersPos[3];
    Vec2 m_answersEndPos[3];
    Actor* m_pMSActor = nullptr;
    Sprite* m_pTopicsSp[4];
    Vec2 m_topicsPos[4];
    Sprite* m_pHintSp = nullptr;
    int m_InitAniIndex = 0;
    Vec2 m_originalPoint;
    Vec2 m_touchBeganPoint;         // 拖放开始位置
    int m_effectId = 0;                //音效play的id
    bool m_opentouched = false;
    bool m_jumped = false;
};

#endif /* defined(__superBaby__Round042_02__) */
