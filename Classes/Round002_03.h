//
//  Round002_03.h
//  AbelLLExplorer
//
//  Created by adinnet on 2015/3/9.
//
//  第二个关卡第三回合
//

#ifndef __AbelLLExplorer__Round002_03__
#define __AbelLLExplorer__Round002_03__

#include "RoundBase.h"
#include <vector>

USING_NS_CC;

class Actor;
class StageScene;
class Round002_03 : public RoundBase
{
public:
    Round002_03();
    virtual ~Round002_03();
    
public:
    bool initWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    static Round002_03* createWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    
public:
    void cleanUp();
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
    /* 本回合结束后蚂蚁 move */
    void antMoveOnComplete();
    /* 庆祝完成 */
    
private:
    Rect m_answerRect;                      // 目标答案矩形
    Node* m_pAntNode = nullptr;             // 放四只蚂蚁的容器结点
    
    Actor* m_pAntActors[4];                 // 存放所有蚂蚁 actor
    Actor* m_pNpcActor = nullptr;
    
    Sprite* m_pArrow = nullptr;             // 指向npc的箭头
    
    Rect m_antRect[4];
    unsigned int m_antPosIndex[4];
    int m_effectId = 0;
    int m_correctNumber = 1;
};

#endif /* defined(__AbelLLExplorer__Round002_03__) */
