//
//  Round046_02.h
//  superBaby
//
//  Created by Administrator on 15/8/4.
//
//

#ifndef __superBaby__Round046_02__
#define __superBaby__Round046_02__

#include "RoundBase.h"

USING_NS_CC;

class Actor;
class StageScene;
class Round046_02 : public RoundBase
{
public:
    Round046_02();
    virtual ~Round046_02();
    
public:
    bool initWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    static Round046_02* createWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    
public:
    void cleanUp();
    virtual void onEnterRound();
    virtual void celebrate(const float);
    /* 开启触摸 */
    virtual void openTouch();
    /*  回合结束回调 */
    virtual void endRound();
    /*  处理npc收集 */
    virtual void processNpcCollect();
    //上升答案
    void upAnswersSchedule(float);
    
    void downAnswers();
    
private:
    Sprite* m_pAnswers[4];
    Vec2 m_answersPos[4];
    Vec2 m_answersEndPos[4];
    Image* m_pImages[4];
    Actor* m_pMSActor= nullptr;
    int m_effectId = 0;                //音效play的id
    int m_InitAniIndex = 0;
    bool m_opentouched = false;
    bool m_jumped = false;
    unsigned int m_curSelIndex = 0;
};

#endif /* defined(__superBaby__Round046_02__) */
