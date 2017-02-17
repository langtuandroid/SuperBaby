//
//  Round035_04.h
//  superBaby
//
//  Created by Administrator on 15/7/6.
//
//

#ifndef __superBaby__Round035_04__
#define __superBaby__Round035_04__

#include "RoundBase.h"
#include <ui/CocosGUI.h>
USING_NS_CC;
using namespace ui;
class Actor;
class StageScene;
class Round035_04 : public RoundBase
{
public:
    Round035_04();
    virtual ~Round035_04();
    
public:
    bool initWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    static Round035_04* createWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    
public:
    void cleanUp();
    virtual void onEnterRound();
    virtual void celebrate(const float);
    /* 开启触摸 */
    virtual void openTouch();
    /*  回合结束回调 */
    virtual void endRound();
    
    //改变花大小
    void changeScaleFlowersSchedule(float);
    
    //改变气泡大小
    void changeScaleBubblesSchedule(float);
    
    //答对后左移鱼
    void actionFishs();
    
private:
    Sprite* m_pFlowersSp[4];            //花
    Vec2 m_flowersPos[4];                //花坐标
    Vec2 m_flowersEndPos[4];
    Sprite* m_pBubbleSp[4];
    Vec2 m_bubblesPos[4];
    int m_effectId = 0;                 //音效play的id
    int m_InitAniIndex = 0;
    bool m_opentouched = false;
    bool m_jumped = false;
    Actor* m_pPaopaoActor = nullptr;
    int m_curLastPosInex = -1;
    Actor* m_pWhiteLightActor = nullptr;
    int m_curCorrectNum = 0;
    Vec2 m_originalPoint;
    Vec2 m_touchBeganPoint;         // 拖放开始位置
};

#endif /* defined(__superBaby__Round035_04__) */
