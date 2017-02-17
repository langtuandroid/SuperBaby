//
//  Round020_01.h
//  AbelLLExplorer
//
//  Created by Administrator on 15/5/20.
//
//

#ifndef __AbelLLExplorer__Round020_01__
#define __AbelLLExplorer__Round020_01__

#include "RoundBase.h"

USING_NS_CC;

class Actor;
class StageScene;
class Round020_01 : public RoundBase
{
public:
    Round020_01();
    virtual ~Round020_01();
    
public:
    bool initWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    static Round020_01* createWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    
public:
    
    
public:
    virtual void onEnterRound();
    virtual void celebrate(const float);
    /* 开启触摸 */
    virtual void openTouch();
    /*  回合结束回调 */
    virtual void endRound();
    
    //改变大小出现
    void changeScalePartsSchedule(float);
    
    //答对后改变actor动作
    void changeActor();
    
    //渐隐消失所有答案
    void fadeAll();
    
private:
    
    Sprite* m_pPartsSp[3];
    Vec2 m_partsPos[3];
    Vec2 m_partsEndPos[3];
    Sprite* m_pTargetPartsSp;
    
    Rect m_targetRect;
    
    unsigned int m_InitAniIndex = 0;
    int m_effectId = 0;
    Actor* m_pRobotActor = nullptr;
    Vec2 m_originalPoint;
    Vec2 m_touchBeganPoint;         // 拖放开始位置
    bool m_opentouched = false;
    bool m_jumped = false;
};


#endif /* defined(__AbelLLExplorer__Round020_01__) */
