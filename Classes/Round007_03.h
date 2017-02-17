//
//  Round007_03.h
//  AbelLLExplorer
//
//  Created by Administrator on 15/4/3.
//
//

#ifndef __AbelLLExplorer__Round007_03__
#define __AbelLLExplorer__Round007_03__

#include "RoundBase.h"
USING_NS_CC;

class Actor;
class StageScene;
class Round007_03 : public RoundBase
{
public:
    Round007_03();
    virtual ~Round007_03();
    
public:
    bool initWithRoundIndex(const int,StageScene*, const Vec2& pos = Vec2::ZERO);
    static Round007_03* createWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    
public:
    virtual void celebrate(const float);
    virtual void onPreLoadedRes();
    virtual void openTouch();
    /*  回合结束回调 */
    virtual void endRound();

    // 回合开场动画，答案时间上升动画调度
    void answersTimeUpSchedule(float);
    // 回合结束动画，答案时间下降动画调度
    void answersTimeDownSchedule(float);
    //答对时加载解除魔法效果gaf
    void addEffectActor();
    
    //答对时改变时钟的动作
    void changeClockActor();
    
private:
    void cleanUp();
    virtual void onEnterRound();
    
private:
    SpriteBatchNode* m_pBN;     //BN
    Sprite* m_pAnswerTimes[3];  //答案时间组
    Actor* m_pClockActor;       //时钟actor
    Actor* m_pFreeEffectActor;  //解除魔法效果actor
    
    Vec2 m_answerTimePos[3];   //答案时间坐标组

    Vec2 m_originalPoint;
    Vec2 m_touchBeganPoint;
    unsigned int m_InitAniIndex;
    int m_effectId;
    bool m_opentouched = false;
    bool m_jumped = false;
};

#endif /* defined(__AbelLLExplorer__Round007_03__) */
