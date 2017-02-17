//
//  Round002_01.h
//  AbelLLExplorer
//
//  Created by adinnet on 2015/3/6.
//
//  第二个关卡第一回合
//

#ifndef __AbelLLExplorer__Round002_01__
#define __AbelLLExplorer__Round002_01__

#include "RoundBase.h"
#include "ui/CocosGUI.h"
USING_NS_CC;
using namespace ui;

class Actor;
class StageScene;
class Round002_01 : public RoundBase
{
public:
    Round002_01();
    virtual ~Round002_01();

public:
    bool initWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    static Round002_01* createWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);

public:
    //上升水果答案
    void upFruitsSchedule(float);
    //渐变消失水果答案
    void fadeOutFruit();
    
    //回合开始，突出树上水果
    void changeFruit();
    
public:
    virtual void onEnterRound();
    virtual void celebrate(const float);
    virtual void onPreLoadedRes();
    /* 开启触摸 */
    virtual void openTouch();
    /*  回合结束回调 */
    virtual void endRound();
    /*  处理npc收集 */
    virtual void processNpcCollect();

private:
    SpriteBatchNode* m_pBN = nullptr;
    
    Sprite* m_fruitSp[4];
    Sprite* m_fruitSpUp[4];
    
    ImageView* m_pTreeFruits;
    unsigned int m_correctNumber = 0;
    unsigned int m_InitAniIndex = 0;
    int m_effectId = 0;
    
    Actor* m_pNpcActor = nullptr;
    bool m_opentouched = false;
    bool m_jumped = false;
};


#endif /* defined(__AbelLLExplorer__Round002_01__) */
