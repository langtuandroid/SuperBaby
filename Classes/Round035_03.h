//
//  Round035_03.h
//  superBaby
//
//  Created by Administrator on 15/7/6.
//
//

#ifndef __superBaby__Round035_03__
#define __superBaby__Round035_03__

#include "RoundBase.h"
#include <ui/CocosGUI.h>
USING_NS_CC;
class Actor;
class StageScene;
class Round035_03 : public RoundBase
{
public:
    Round035_03();
    virtual ~Round035_03();
    
public:
    bool initWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    static Round035_03* createWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    
public:
    void cleanUp();
    virtual void onEnterRound();
    virtual void celebrate(const float);
    /* 开启触摸 */
    virtual void openTouch();
    /*  回合结束回调 */
    virtual void endRound();
    //下降出现瓶子
    void downFadeInBottlesSchedule(float);
    
    //依次放大提示瓶子
    void changeScaleBottlesSchedule(float);
    
    //答对后上升隐藏瓶子
    void upFadeOutSchedule(float);
    
private:
    ui::ImageView* m_pBottlesImageView[5];        //船
    Vec2 m_bottlesPos[5];
    int m_effectId = 0;                //音效play的id
    int m_InitAniIndex = 0;
    bool m_opentouched = false;
    bool m_jumped = false;
    int m_curCorrectNum = 0;
    Actor* m_pPaopaoActor = nullptr;
    Actor* m_pWhiteLightActor = nullptr;
};


#endif /* defined(__superBaby__Round035_03__) */
