//
//  Round032_02.h
//  AbelLLExplorer
//
//  Created by Administrator on 15/6/9.
//
//

#ifndef __AbelLLExplorer__Round032_02__
#define __AbelLLExplorer__Round032_02__

#include "RoundBase.h"

USING_NS_CC;

class Actor;
class StageScene;
class Round032_02 : public RoundBase
{
public:
    Round032_02();
    virtual ~Round032_02();
    
public:
    bool initWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    static Round032_02* createWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    
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
    
    void downAnswersSchedule(float);
    
    void moveMSActor();
    
private:
    Sprite* m_pAnswersSp[3];        //船
    Vec2 m_answersPos[3];
    Actor* m_pMSActor = nullptr;
    int m_effectId = 0;                //音效play的id
    int m_InitAniIndex;
    bool m_opentouched = false;
    bool m_jumped = false;
    Actor* m_pPaopaoActor = nullptr;
    Actor* m_pWhiteLightActor = nullptr;
};


#endif /* defined(__AbelLLExplorer__Round032_02__) */
