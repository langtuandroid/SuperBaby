//
//  Round044_03.h
//  superBaby
//
//  Created by Administrator on 15/8/3.
//
//

#ifndef __superBaby__Round044_03__
#define __superBaby__Round044_03__

#include "RoundBase.h"

USING_NS_CC;

class Actor;
class StageScene;
class Round044_03 : public RoundBase
{
public:
    Round044_03();
    virtual ~Round044_03();
    
public:
    bool initWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    static Round044_03* createWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    
public:
    void cleanUp();
    virtual void onEnterRound();
    virtual void celebrate(const float);
    /* 开启触摸 */
    virtual void openTouch();
    /*  回合结束回调 */
    virtual void endRound();
    
    //依次放大画以提示
    void changeScalePicturesSchedule(float);
    void drawtest(const Rect& rect);
private:
    Actor* m_pGouActor = nullptr;
    Sprite* m_pPicturesSp[2];
    Vec2 m_picturesPos[2];
    Vec2 m_leftHouseGouxuanPos;
    Vec2 m_rightHouseGouxuanPos;
    Rect m_touchLeftRect;
    Rect m_touchRightRect;
    Rect m_touchArea;
    int m_InitAniIndex = 0;
    
    int m_effectId = 0;                //音效play的id
    bool m_opentouched = false;
    bool m_jumped = false;
};

#endif /* defined(__superBaby__Round044_03__) */
