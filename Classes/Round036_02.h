//
//  Round036_02.h
//  superBaby
//
//  Created by Administrator on 15/7/7.
//
//

#ifndef __superBaby__Round036_02__
#define __superBaby__Round036_02__

#include "RoundBase.h"

USING_NS_CC;

class Actor;
class StageScene;
class Round036_02 : public RoundBase
{
public:
    Round036_02();
    virtual ~Round036_02();
    
public:
    bool initWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    static Round036_02* createWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    
public:
    void cleanUp();
    virtual void onEnterRound();
    virtual void celebrate(const float);
    /* 开启触摸 */
    virtual void openTouch();
    /*  回合结束回调 */
    virtual void endRound();
    //依次放大腿以提示
    void changeMaxScaleLegsSchedule(float);
    
    //依次缩小大腿
    void changeSmallScaleLegsSchedule(float);
    
    //答对后改变actor
    void changeActor();
    
private:
    Actor* m_pLegActor = nullptr;
    Sprite* m_pLegsSp[3];
    Vec2 m_legsPos[3];
    
    int m_effectId = 0;                //音效play的id
    int m_InitAniIndex = 0;
    bool m_opentouched = false;
    bool m_jumped = false;
    Actor* m_pPaopaoActor = nullptr;
    Actor* m_pWhiteLightActor = nullptr;
};


#endif /* defined(__superBaby__Round036_02__) */
