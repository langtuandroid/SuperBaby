//
//  Round019_01.h
//  AbelLLExplorer
//
//  Created by Administrator on 15/5/19.
//
//

#ifndef __AbelLLExplorer__Round019_01__
#define __AbelLLExplorer__Round019_01__

#include "RoundBase.h"

USING_NS_CC;

class Actor;
class StageScene;
class Round019_01 : public RoundBase
{
public:
    Round019_01();
    virtual ~Round019_01();
    
public:
    bool initWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    static Round019_01* createWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    
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
    
    //渐隐消失所有答案
    void fadeAll();
    
private:
    
    Sprite* m_pGraphsSp[3];
    Vec2 m_graphsPos[3];
    Vec2 m_graphsEndPos[3];
    Sprite* m_pTargetGraphsSp[3];
    Vec2 m_targetGraphsPos[3];
    Vec2 m_targetGraphsEndPos[3];
    unsigned int m_InitAniIndex = 0;
    int m_effectId = 0;
    Actor* m_pStageActor = nullptr;
    Actor* m_pPillarActor = nullptr;
    Vec2 m_originalPoint;
    Vec2 m_touchBeganPoint;         // 拖放开始位置
    unsigned int m_curCorrectNumber = 0;
    bool m_opentouched = false;
    bool m_jumped = false;
};

#endif /* defined(__AbelLLExplorer__Round019_01__) */
