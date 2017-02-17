//
//  Round028_04.h
//  AbelLLExplorer
//
//  Created by Administrator on 15/6/4.
//
//

#ifndef __AbelLLExplorer__Round028_04__
#define __AbelLLExplorer__Round028_04__

#include "RoundBase.h"

USING_NS_CC;

class Actor;
class StageScene;
class Round028_04 : public RoundBase
{
public:
    Round028_04();
    virtual ~Round028_04();
    
public:
    bool initWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    static Round028_04* createWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    
public:
    void cleanUp();
    virtual void onEnterRound();
    virtual void celebrate(const float);
    /* 开启触摸 */
    virtual void openTouch();
    /*  回合结束回调 */
    virtual void endRound();
    
    //螃蟹依次改变大小以提示
    void changeScaleCrabsSchedule(float);
    
    //答对后右移球
    void actionCrabs();
    
    //答对后改变actor
    void changeActor();
    
private:
    Actor* m_pCrabsActor[4];            //螃蟹
    Vec2 m_crabsPos[4];                 //螃蟹坐标
    int m_effectId = 0;                 //音效play的id
    int m_InitAniIndex = 0;
    bool m_opentouched = false;
    bool m_jumped = false;
    Actor* m_pPaopaoActor = nullptr;
    int m_curLastPosInex = -1;
    Actor* m_pWhiteLightActor = nullptr;

};

#endif /* defined(__AbelLLExplorer__Round028_04__) */
