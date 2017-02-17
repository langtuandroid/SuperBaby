//
//  Round012_02.h
//  AbelLLExplorer
//
//  Created by Administrator on 2015/4/9.
//
//

#ifndef __AbelLLExplorer__Round012_02__
#define __AbelLLExplorer__Round012_02__

#include "RoundBase.h"
USING_NS_CC;

class Actor;
class StageScene;
class Round012_02 : public RoundBase
{
public:
    Round012_02();
    virtual ~Round012_02();
    
public:
    bool initWithRoundIndex(const int,StageScene*, const Vec2& pos = Vec2::ZERO);
    static Round012_02* createWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    
public:
    virtual void celebrate(const float);
    virtual void onEnterRound();
    virtual void onPreLoadedRes();
    virtual void openTouch();
    /*  回合结束回调 */
    virtual void endRound();

    //回合开始，水果蔬菜依次出现
    void fruitAndGreensFadeInSchedule(float);
    
    //回合开始，水果蔬菜依次移动
    void fruitAndGreensMoveSchedule(float);
    
    //答对题改变青蛙actor,吃东西
    void changeActorEatFood();
    
    //当食物移动到目标区域，改变青蛙动作，张嘴
    void changeActorOpenMouth();
    
    //当食物移动离开目标区域，改变青蛙动作，闭嘴
    void changeActorCloseMouth();
    
    //全部答对，青蛙跳水
    void changeActorDive();
    
    //全部答对后，剩余食物上升渐隐消失
    void foodUpFadeOutSchedule(float);
    
    //主角行走到桥
    void actorMovBridge();
    
private:
    SpriteBatchNode* m_pBN = nullptr;
    Sprite* m_basketSp = nullptr;
    
    Sprite* m_foodSps[5];
    
    Actor* m_pFrogActor = nullptr;         //青蛙actor
    
    Rect m_rect = Rect(900.0f, 200.0f, 400.0f, 500.0f);     // 触摸响应区
    Vec2 m_centerRectsPos = Vec2(1100.0f, 450.0f);          //触摸区中点坐标

    Vec2 m_originalPoint;
    Vec2 m_touchBeganPoint;
    int m_InitAniIndex = 0;
    unsigned int m_correctNumber = 0;
    int m_effectId = 0;
    Vec2 m_originalPoints[5];
};

#endif /* defined(__AbelLLExplorer__Round012_02__) */
