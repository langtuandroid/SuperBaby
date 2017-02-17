//
//  Round033_01.h
//  AbelLLExplorer
//
//  Created by Administrator on 15/6/10.
//
//

#ifndef __AbelLLExplorer__Round033_01__
#define __AbelLLExplorer__Round033_01__

#include "RoundBase.h"
USING_NS_CC;
class Actor;
class StageScene;
class Round033_01 : public RoundBase
{
public:
    Round033_01();
    virtual ~Round033_01();
    
public:
    bool initWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    static Round033_01* createWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    
public:
    
    
public:
    virtual void onEnterRound();
    virtual void celebrate(const float);
    /* 开启触摸 */
    virtual void openTouch();
    /*  回合结束回调 */
    virtual void endRound();
    
    //改变答案大小以提示
    void changeScaleAnswersSchedule(float);
    
private:
    Sprite* m_pAnswersSp[3];
    Vec2 m_answersPos[3];
    bool m_opentouched = false;
    bool m_jumped = false;
    unsigned int m_InitAniIndex = 0;
    int m_effectId = 0;
    Vec2 m_originalPoint;
    Vec2 m_touchBeganPoint;         // 拖放开始位置
    Actor* m_pPaopaoActor = nullptr;
    Actor* m_pWhiteLightActor = nullptr;
};

#endif /* defined(__AbelLLExplorer__Round033_01__) */
