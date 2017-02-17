//
//  Round028_03.h
//  AbelLLExplorer
//
//  Created by Administrator on 15/6/4.
//
//

#ifndef __AbelLLExplorer__Round028_03__
#define __AbelLLExplorer__Round028_03__

#include "RoundBase.h"

USING_NS_CC;

class Actor;
class StageScene;
class Round028_03 : public RoundBase
{
public:
    Round028_03();
    virtual ~Round028_03();
    
public:
    bool initWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    static Round028_03* createWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    
public:
    void cleanUp();
    virtual void onEnterRound();
    virtual void celebrate(const float);
    /* 开启触摸 */
    virtual void openTouch();
    /*  回合结束回调 */
    virtual void endRound();
    //依次放大鱼以提示
    void changeScalefishsSchedule(float);
    
    //答对移动鱼
    void moveFish();
    
private:
    Actor* m_pFishsActor[5];        //船
    Vec2 m_fishsPos[5];
    int m_effectId = 0;                //音效play的id
    int m_InitAniIndex = 0;
    bool m_opentouched = false;
    bool m_jumped = false;
    int m_curCorrectNum = 0;
    Actor* m_pPaopaoActor = nullptr;
    Actor* m_pWhiteLightActor = nullptr;

};


#endif /* defined(__AbelLLExplorer__Round028_03__) */
