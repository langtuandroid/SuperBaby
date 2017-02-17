//
//  Round027_03.h
//  AbelLLExplorer
//
//  Created by Administrator on 15/6/2.
//
//

#ifndef __AbelLLExplorer__Round027_03__
#define __AbelLLExplorer__Round027_03__

#include "RoundBase.h"

USING_NS_CC;

class Actor;
class StageScene;
class Round027_03 : public RoundBase
{
public:
    Round027_03();
    virtual ~Round027_03();
    
public:
    bool initWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    static Round027_03* createWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    
public:
    void cleanUp();
    virtual void onEnterRound();
    virtual void celebrate(const float);
    /* 开启触摸 */
    virtual void openTouch();
    /*  回合结束回调 */
    virtual void endRound();
    
    //球出现
    void fadeScaleBallsSchedule(float);
    
    //答对后右移球
    void actionBallsSchedule();
    
private:
    Sprite* m_pBallsSp[3];            //答案图片
    Vec2 m_ballsPos[3];               //答案坐标
    int m_effectId = 0;                 //音效play的id
    int m_curSelIndex;                  //选中树索引
    int m_InitAniIndex = 0;
    bool m_opentouched = false;
    bool m_jumped = false;
    Actor* m_pBoguangActor = nullptr;
    unsigned int m_randNums[3];
};

#endif /* defined(__AbelLLExplorer__Round027_03__) */
