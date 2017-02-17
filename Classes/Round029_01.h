//
//  Round029_01.h
//  AbelLLExplorer
//
//  Created by Administrator on 15/6/4.
//
//

#ifndef __AbelLLExplorer__Round029_01__
#define __AbelLLExplorer__Round029_01__

#include "RoundBase.h"
USING_NS_CC;
class Actor;
class StageScene;
class Round029_01 : public RoundBase
{
public:
    Round029_01();
    virtual ~Round029_01();
    
public:
    bool initWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    static Round029_01* createWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    
public:
    
    
public:
    virtual void onEnterRound();
    virtual void celebrate(const float);
    /* 开启触摸 */
    virtual void openTouch();
    /*  回合结束回调 */
    virtual void endRound();
    /*  处理npc收集 */
    virtual void processNpcCollect();
    
    //上升答案块
    void upAnswersSchedule(float);
    
    void fadeHintSp();
    
    void fadeAnswer();
    
private:
    Actor* m_pNpcActor = nullptr;
    Sprite* m_pAnswersSp[3];
    Vec2 m_answersPos[3];
    Vec2 m_answersEndPos[3];
    Sprite* m_pHintSp = nullptr;
    Sprite* m_pBeganTopicSp= nullptr;
    Sprite* m_pEndTopicSp = nullptr;
    bool m_opentouched = false;
    bool m_jumped = false;
    unsigned int m_InitAniIndex = 0;
    int m_effectId = 0;
    Vec2 m_originalPoint;
    Vec2 m_touchBeganPoint;         // 拖放开始位置
    Actor* m_pPaopaoActor = nullptr;
    Actor* m_pWhiteLightActor = nullptr;
};

#endif /* defined(__AbelLLExplorer__Round029_01__) */
