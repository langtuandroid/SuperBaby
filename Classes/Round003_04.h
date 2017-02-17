//
//  Round003_04.h
//  AbelLLExplorer
//
//  Created by adinnet on 2015/3/6.
//
//  第 3 关卡第 4 回合
//

#ifndef __AbelLLExplorer__Round003_04__
#define __AbelLLExplorer__Round003_04__

#include "RoundBase.h"
#include <vector>

USING_NS_CC;

class Actor;
class StageScene;
class Round003_04 : public RoundBase
{
public:
    Round003_04();
    virtual ~Round003_04();
    
public:
    bool initWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    static Round003_04* createWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    
public:
    virtual void onEnterRound();
    virtual void celebrate(const float);
    virtual void onPreLoadedRes();
    /* 开启触摸 */
    virtual void openTouch();
    /*  回合结束回调 */
    virtual void endRound();

private:
    SpriteBatchNode* m_pSBN = nullptr;
    
    Sprite* m_pQuestionSps[4];          // 题目精灵
    
    Vec2 m_originalPoint;               // 原来的位置
    Vec2 m_touchBeganPoint;             // 拖放开始位置
    
    Node* m_pOverActorNode = nullptr;
    Sprite* m_pTree7Sp = nullptr;
    int m_effectId = 0;
    
    Vec2 m_originalPoints[4];
};

#endif /* defined(__AbelLLExplorer__Round003_04__) */
