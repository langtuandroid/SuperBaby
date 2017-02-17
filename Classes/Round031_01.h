//
//  Roung031_01.h
//  AbelLLExplorer
//
//  Created by Administrator on 15/6/8.
//
//

#ifndef __AbelLLExplorer__Round031_01__
#define __AbelLLExplorer__Round031_01__

#include "RoundBase.h"
USING_NS_CC;
class Actor;
class StageScene;
class Round031_01 : public RoundBase
{
public:
    Round031_01();
    virtual ~Round031_01();
    
public:
    bool initWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    static Round031_01* createWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    
public:
    
    
public:
    virtual void onEnterRound();
    virtual void celebrate(const float);
    /* 开启触摸 */
    virtual void openTouch();
    /*  回合结束回调 */
    virtual void endRound();
    
    //改变提示大小
    void changeScaleHintSchedule(float);
    
    //改变答案大小以提示
    void changeScaleAnswersSchedule(float);
    
    void moveFishes();
    
private:
    Actor* m_pNpcActor = nullptr;
    Actor* m_pFishesActor[10];
    Vec2 m_fishesPos[10];
    Actor* m_pAnswersActor[2];
    Vec2 m_answersPos[2];
    Vec2 m_answersEndPos[2];
    Sprite* m_pHintSp[2];
    Sprite* m_pHighLightHintSp[2];
    Sprite* m_pMarksSp[2];
    Vec2 m_hintsPos[2];
    bool m_opentouched = false;
    bool m_jumped = false;
    unsigned int m_InitAniIndex = 0;
    int m_effectId = 0;
    Vec2 m_originalPoint;
    Vec2 m_touchBeganPoint;         // 拖放开始位置
    Actor* m_pPaopaoActor = nullptr;
    Actor* m_pWhiteLightActor = nullptr;
    unsigned int m_curCorrectNum = 0;
};

#endif /* defined(__AbelLLExplorer__Roung031_01__) */
