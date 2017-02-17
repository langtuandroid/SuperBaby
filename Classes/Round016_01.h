//
//  Round016_01.h
//  AbelLLExplorer
//
//  Created by Administrator on 15/5/15.
//
//

#ifndef __AbelLLExplorer__Round016_01__
#define __AbelLLExplorer__Round016_01__

#include "RoundBase.h"

USING_NS_CC;

class Actor;
class StageScene;
class Round016_01 : public RoundBase
{
public:
    Round016_01();
    virtual ~Round016_01();
    
public:
    bool initWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    static Round016_01* createWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    
public:
    
    
public:
    virtual void onEnterRound();
    virtual void celebrate(const float);
    /* 开启触摸 */
    virtual void openTouch();
    /*  回合结束回调 */
    virtual void endRound();
    /*  处理npc收集 */
    virtual void processNpcCollect();
    
    //改变大小出现
    void changeScaleFoodsSchedule(float);
    
    //提示
    void changeScaleFoodsHintSchedule(float);
    
    //答对后改变actor动作
    void changeNpcActor();
    
    //开启吊车门
    void openDoor();
    
    //上升移动门
    void upMoveDoor();
    
    //答对后隐藏剩余食物
    void fadeOutFoods();
    
private:
    SpriteBatchNode* m_pBN = nullptr;

    Sprite* m_pFoodsSp[4];
    
    Vec2 m_foodsPos[4];
    Vec2 m_foodsEndPos[4];
    
    Vec2 m_originalPoint;
    Vec2 m_touchBeganPoint;         // 拖放开始位置
    unsigned int m_InitAniIndex = 0;
    int m_effectId = 0;
    Actor* m_pNpcActor = nullptr;
    Actor* m_pCraneActor = nullptr;
    unsigned int m_curRandThirdPosFoodIndex = 0;        //当前随机出第三个位置时所对应的食物的index
    bool m_opentouched = false;
    bool m_jumped = false;
};

#endif /* defined(__AbelLLExplorer__Round016_01__) */
