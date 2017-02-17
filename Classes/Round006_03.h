//
//  Round006_03.h
//  AbelLLExplorer
//
//  Created by Administrator on 15/4/2.
//
//

#ifndef __AbelLLExplorer__Round006_03__
#define __AbelLLExplorer__Round006_03__

#include "RoundBase.h"
USING_NS_CC;

class Actor;
class StageScene;
class Round006_03 : public RoundBase
{
public:
    Round006_03();
    virtual ~Round006_03();
    
public:
    bool initWithRoundIndex(const int,StageScene*, const Vec2& pos = Vec2::ZERO);
    static Round006_03* createWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    
public:
    virtual void celebrate(const float);
    virtual void onPreLoadedRes();
    /* 开启触摸 */
    virtual void openTouch();
    /*  回合结束回调 */
    virtual void endRound();
    // 回合开场动画，木板和颜色下降动画调度
    void colorBoardDown();
    //依次放大颜色
    void changeScaleColorsSchedule(float);
    
    //显示提示
    void fadeHint();
    
    //选对后颜色板上升
    void colorBoardUp();
    
    //选对后改变虫的动作
    void changeWorm();
    
private:
    void cleanUp();
    virtual void onEnterRound();
    
private:
    SpriteBatchNode* m_pColorAndBoardBN;        //颜色和木板的BN
    Sprite* m_pColors[3];                       //颜色组
    Sprite* m_pBoard;                           //木板
    Sprite* m_pHintSp = nullptr;
    Vec2 m_colorPos[3];                         //颜色位置组
    Vec2 m_colorEndPos[3];
    Actor* m_pWormActor;                        //虫的actor
    int m_effectId;
    Vec2 m_originalPoint;
    Vec2 m_touchBeganPoint;
    int m_InitAniIndex = 0;
    bool m_opentouched = false;
    bool m_jumped = false;
};

#endif /* defined(__AbelLLExplorer__Round006_03__) */
