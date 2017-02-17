//
//  Round031_02.h
//  AbelLLExplorer
//
//  Created by Administrator on 15/6/8.
//
//

#ifndef __AbelLLExplorer__Round031_02__
#define __AbelLLExplorer__Round031_02__

#include "RoundBase.h"

USING_NS_CC;

class Actor;
class StageScene;
class Round031_02 : public RoundBase
{
public:
    Round031_02();
    virtual ~Round031_02();
    
public:
    bool initWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    static Round031_02* createWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    
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
    //改变大小以提示
    void changeScaleAnswersSchedule(float);
    //改变大小提示题目
    void changeScaleTopic();
    
    void changeActor();
    
    void downAnswersSchedule(float);
    
private:
    Sprite* m_pAnswersSp[3];        //船
    Vec2 m_answersPos[3];
    Sprite* m_pShellSp = nullptr;
    Sprite* m_pHighLightShellSp = nullptr;
    Actor* m_pMSActor = nullptr;
    int m_effectId = 0;                //音效play的id
    int m_InitAniIndex;
    bool m_opentouched = false;
    bool m_jumped = false;
    Actor* m_pPaopaoActor = nullptr;
    Actor* m_pWhiteLightActor = nullptr;
};

#endif /* defined(__AbelLLExplorer__Round031_02__) */
