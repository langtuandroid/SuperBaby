//
//  Round026_01.h
//  AbelLLExplorer
//
//  Created by Administrator on 15/6/1.
//
//

#ifndef __AbelLLExplorer__Round026_01__
#define __AbelLLExplorer__Round026_01__

#include "RoundBase.h"

USING_NS_CC;

class Actor;
class StageScene;
class Round026_01 : public RoundBase
{
public:
    Round026_01();
    virtual ~Round026_01();
    
public:
    bool initWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    static Round026_01* createWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    
public:
    void cleanUp();
    virtual void onEnterRound();
    virtual void celebrate(const float);
    /* 开启触摸 */
    virtual void openTouch();
    /*  回合结束回调 */
    virtual void endRound();
    
    //依次放大城堡以提示
    void changeScaleHousesSchedule(float);
    void drawtest(const Rect& rect);
private:
    Actor* m_pGouActor = nullptr;
    Sprite* m_pHousesSp[2];
    Vec2 m_housesPos[2];
    Vec2 m_leftHouseGouxuanPos[3];
    Vec2 m_rightHouseGouxuanPos[3];
    Rect m_touchLeftRects[3];
    Rect m_touchRightRects[3];
    Rect m_touchArea;
    bool m_answered[3];
    int m_InitAniIndex;
    
    int m_effectId = 0;                //音效play的id
    bool m_opentouched = false;
    bool m_jumped = false;
    std::vector<Actor*> m_pGouxuanActors;
    Actor* m_pBoguangActor = nullptr;
};

#endif /* defined(__AbelLLExplorer__Round026_01__) */
