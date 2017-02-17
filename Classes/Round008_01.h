//
//  Round008_01.h
//  AbelLLExplorer
//
//  Created by Administrator on 2015/4/7.
//
//

#ifndef __AbelLLExplorer__Round008_01__
#define __AbelLLExplorer__Round008_01__

#include "RoundBase.h"
USING_NS_CC;

class Actor;
class StageScene;
class Round008_01 : public RoundBase
{
public:
    Round008_01();
    virtual ~Round008_01();
    
public:
    bool initWithRoundIndex(const int,StageScene*, const Vec2& pos = Vec2::ZERO);
    static Round008_01* createWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    
public:
    virtual void celebrate(const float);
    virtual void onPreLoadedRes();
    virtual void openTouch();
    /*  回合结束回调 */
    virtual void endRound();

    //开始回合，渐变出现钱币答案
    void fadeInMoneysSchedule(float);
    //改变答案尺寸以突出
    void changeScaleMoneysSchedule(float);
    
    //离开回合，渐变隐藏钱币答案
    void fadeOutMoneysSchedule(float);
    //答对时改变鸡的动作
    void changeChickenActor();
    
private:
    void cleanUp();
    virtual void onEnterRound();
    
private:
    SpriteBatchNode* m_pBN;     //BN
    Sprite* m_pMoneys[3];       //答案钱币组
    Actor* m_pStandbyChickenActor;     //待机状态母鸡actor
    Actor* m_pTriggerChickenActor;     //触发状态母鸡actor
    
    Vec2 m_moneysPos[3];        //答案钱币坐标组

    unsigned int m_InitAniIndex;
    int m_effectId;
    bool m_opentouched = false;
    bool m_jumped = false;
};

#endif /* defined(__AbelLLExplorer__Round008_01__) */
