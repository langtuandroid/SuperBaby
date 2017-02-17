//
//  Round038_01.h
//  superBaby
//
//  Created by Administrator on 15/7/21.
//
//

#ifndef __superBaby__Round038_01__
#define __superBaby__Round038_01__

#include "RoundBase.h"

USING_NS_CC;

class Actor;
class StageScene;
class Round038_01 : public RoundBase
{
public:
    Round038_01();
    virtual ~Round038_01();
    
public:
    bool initWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    static Round038_01* createWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    
public:
    void cleanUp();
    virtual void onEnterRound();
    virtual void celebrate(const float);
    /* 开启触摸 */
    virtual void openTouch();
    /*  回合结束回调 */
    virtual void endRound();
    /*  处理npc收集 */
    virtual void processNpcCollect();
    //工具依次出现
    void scaleChangeToolSchedule(float);
    
    //答对后渐隐
    void fadeOutTools();
    
private:
    Sprite* m_pToolsSp[3];               //工具
    Vect m_toolsPos[3];                //工具的坐标
    Actor* m_pNpcActor = nullptr;
    int m_InitAniIndex = 0;
    bool m_opentouched = false;
    bool m_jumped = false;
    int m_effectId = 0;                //音效play的id
};


#endif /* defined(__superBaby__Round038_01__) */
