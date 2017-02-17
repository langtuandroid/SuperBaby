//
//  Round030_02.h
//  AbelLLExplorer
//
//  Created by Administrator on 15/6/8.
//
//

#ifndef __AbelLLExplorer__Round030_02__
#define __AbelLLExplorer__Round030_02__

#include "RoundBase.h"

USING_NS_CC;

class Actor;
class StageScene;
class Round030_02 : public RoundBase
{
public:
    Round030_02();
    virtual ~Round030_02();
    
public:
    bool initWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    static Round030_02* createWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    
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
    //改变大小以提示
    void changeScaleHint();
private:
    Sprite* m_pAnswersSp[3];
    Sprite* m_pHighLightAnswersSp[3];
    Vec2 m_answersPos[3];
    Sprite* m_pSeaHorsesSp[3];
    Vec2 m_seaHorsesPos[3];
    Sprite* m_pRulersSp[3];
    Vec2 m_rulersPos[3];
    Sprite* m_pSticksSp[3];
    Vec2 m_sticksPos[3][3];
    int m_InitAniIndex = 0;
    int m_effectId = 0;                //音效play的id
    bool m_opentouched = false;
    bool m_jumped = false;
    Actor* m_pPaopaoActor = nullptr;
    Actor* m_pWhiteLightActor = nullptr;
    unsigned int m_curRandNum = 0;
};

#endif /* defined(__AbelLLExplorer__Round030_02__) */
