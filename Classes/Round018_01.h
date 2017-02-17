//
//  Round018_01.h
//  AbelLLExplorer
//
//  Created by Administrator on 15/5/18.
//
//

#ifndef __AbelLLExplorer__Round018_01__
#define __AbelLLExplorer__Round018_01__

#include "RoundBase.h"

USING_NS_CC;

class Actor;
class StageScene;
class Round018_01 : public RoundBase
{
public:
    Round018_01();
    virtual ~Round018_01();
    
public:
    bool initWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    static Round018_01* createWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    
public:
    
    
public:
    virtual void onEnterRound();
    virtual void celebrate(const float);
    /* 开启触摸 */
    virtual void openTouch();
    /*  回合结束回调 */
    virtual void endRound();
    
    //改变大小出现
    void changeScaleFlowersSchedule(float);
    
    //答对后改变actor动作
    void changeActor();
    
    //答对后移动花
    void moveFlower();
    
private:
    
    Sprite* m_pFlowersSp[4];
    
    Vec2 m_flowersPos[4];
    
    unsigned int m_InitAniIndex = 0;
    int m_effectId = 0;
    Actor* m_pMSActor = nullptr;
    bool m_opentouched = false;
    bool m_jumped = false;
};

#endif /* defined(__AbelLLExplorer__Round018_01__) */
