//
//  Round013_03.h
//  AbelLLExplorer
//
//  Created by adinnet on 2015/3/9.
//
//

#ifndef __AbelLLExplorer__Round013_03__
#define __AbelLLExplorer__Round013_03__

#include "RoundBase.h"

USING_NS_CC;

class Actor;
class LeadingActor;
class StageScene;
class Round013_03 : public RoundBase
{
public:
    Round013_03();
    virtual ~Round013_03();
    
public:
    bool initWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    static Round013_03* createWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    
public:
    virtual void onEnterRound();
    virtual void celebrate(const float);
    virtual void onPreLoadedRes();
    /* 开启触摸 */
    virtual void openTouch();
    /*  回合结束回调 */
    virtual void endRound();
    /* 处理 npc 收集 */
    virtual void processNpcCollect();

private:
    /* 缩放完成回调 */
    void scaleCompleteCB();
    
private:
    SpriteBatchNode* m_pSBN = nullptr;
    Sprite* m_robotSp[4];
    Sprite* m_robotSp1[4];
    Vec2 m_robotPos[4];
    
    int scaleIndex[4];
    unsigned int m_numberIndex = 0;
    
    Actor* m_pNpcActor= nullptr;
    int m_effectId = 0;                //音效play的id
};

#endif /* defined(__AbelLLExplorer__Round013_03__) */
