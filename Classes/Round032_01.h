//
//  Round032_01.h
//  AbelLLExplorer
//
//  Created by Administrator on 15/6/9.
//
//

#ifndef __AbelLLExplorer__Round032_01__
#define __AbelLLExplorer__Round032_01__

#include "RoundBase.h"
USING_NS_CC;
class Actor;
class StageScene;
class Round032_01 : public RoundBase
{
public:
    Round032_01();
    virtual ~Round032_01();
    
public:
    bool initWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    static Round032_01* createWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    
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
    
    //提示答案
    void changeScaleAnswersSchedule(float);
    
    void fadeAnswer();
    
    void changeActor();
    
private:
    Actor* m_pNpcActor = nullptr;
    Sprite* m_pAnswersSp[3];
    Vec2 m_answersPos[3];
    Vec2 m_answersEndPos[3];
    bool m_opentouched = false;
    bool m_jumped = false;
    unsigned int m_InitAniIndex = 0;
    int m_effectId = 0;
    Actor* m_pPaopaoActor = nullptr;
    Actor* m_pWhiteLightActor = nullptr;
};


#endif /* defined(__AbelLLExplorer__Round032_01__) */
