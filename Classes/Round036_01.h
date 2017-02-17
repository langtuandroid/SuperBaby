//
//  Round036_01.h
//  superBaby
//
//  Created by Administrator on 15/7/7.
//
//

#ifndef __superBaby__Round036_01__
#define __superBaby__Round036_01__

#include "RoundBase.h"

USING_NS_CC;

class Actor;
class StageScene;
class Round036_01 : public RoundBase
{
public:
    Round036_01();
    virtual ~Round036_01();
    
public:
    bool initWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    static Round036_01* createWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    
public:
    void cleanUp();
    virtual void onEnterRound();
    virtual void celebrate(const float);
    /* 开启触摸 */
    virtual void openTouch();
    /*  回合结束回调 */
    virtual void endRound();
    
    //依次放大鱼以提示
    void changeScaleFishesSchedule(float);
    
    //答对后移动鱼
    void moveFishes();
    
    void drawtest(const Rect& rect);
private:
    Actor* m_pGouActor = nullptr;
    Sprite* m_pFishesSp[2];
    Vec2 m_fishesPos[2];
    Vec2 m_leftHouseGouxuanPos[2];
    Vec2 m_rightHouseGouxuanPos[2];
    Rect m_touchLeftRects[2];
    Rect m_touchRightRects[2];
    Rect m_touchArea;
    bool m_answered[2];
    int m_InitAniIndex;
    
    int m_effectId = 0;                //音效play的id
    bool m_opentouched = false;
    bool m_jumped = false;
    std::vector<Actor*> m_pGouxuanActors;
    Actor* m_pPaopaoActor = nullptr;
    Actor* m_pWhiteLightActor = nullptr;
};

#endif /* defined(__superBaby__Round036_01__) */
