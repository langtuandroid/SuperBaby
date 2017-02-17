//
//  Round040_01.h
//  superBaby
//
//  Created by Administrator on 15/7/24.
//
//

#ifndef __superBaby__Round040_01__
#define __superBaby__Round040_01__

#include "RoundBase.h"
#include <ui/CocosGUI.h>
USING_NS_CC;

class Actor;
class StageScene;
class Round040_01 : public RoundBase
{
public:
    Round040_01();
    virtual ~Round040_01();
    
public:
    bool initWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    static Round040_01* createWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    
public:
    void cleanUp();
    virtual void onEnterRound();
    virtual void celebrate(const float);
    /* 开启触摸 */
    virtual void openTouch();
    /*  回合结束回调 */
    virtual void endRound();
    
    //依次放大雕塑以显示
    void changeScaleSculpturesSchedule(float);
private:
    Sprite* m_pSculpturesSp[2];
    Vec2 m_sculpturesPos[2];
    Sprite* m_pDifSp[2];
    Vec2 m_difsPos[2];
    Vec2 m_leftHouseGouxuanPos[2];
    Vec2 m_rightHouseGouxuanPos[2];
    Rect m_touchLeftRects[2];
    Rect m_touchRightRects[2];
    Rect m_touchArea;
    bool m_answered[2];
    int m_InitAniIndex = 0;
    int m_effectId = 0;                //音效play的id
    bool m_opentouched = false;
    bool m_jumped = false;
    Actor* m_pGouActor = nullptr;
    std::vector<Actor*> m_pGouxuanActors;
};


#endif /* defined(__superBaby__Round040_01__) */
