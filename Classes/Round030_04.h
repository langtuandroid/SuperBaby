//
//  Round030_04.h
//  AbelLLExplorer
//
//  Created by Administrator on 15/6/8.
//
//

#ifndef __AbelLLExplorer__Round030_04__
#define __AbelLLExplorer__Round030_04__

#include "RoundBase.h"
#include <ui/CocosGUI.h>
USING_NS_CC;
using namespace ui;
class Actor;
class StageScene;
class Round030_04 : public RoundBase
{
public:
    Round030_04();
    virtual ~Round030_04();
    
public:
    bool initWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    static Round030_04* createWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    
public:
    void cleanUp();
    virtual void onEnterRound();
    virtual void celebrate(const float);
    /* 开启触摸 */
    virtual void openTouch();
    /*  回合结束回调 */
    virtual void endRound();
    
    //改变鱼大小
    void changeScaleFishs();
    
    //改变气泡大小
    void changeScaleBubblesSchedule(float);
    
    //答对后左移鱼
    void actionFishs();
    
private:
    Actor* m_pFishsActor[3];            //鱼
    Vec2 m_fishsPos[3];                 //鱼坐标
    Vec2 m_fishsEndPos[3];
    Sprite* m_pBubbleSp[3];
    Sprite* m_pHighlightBubbleSp[3];
    Vec2 m_bubblesPos[3];
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

#endif /* defined(__AbelLLExplorer__Round030_04__) */
