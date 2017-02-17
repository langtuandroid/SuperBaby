//
//  Round025_01.h
//  AbelLLExplorer
//
//  Created by Administrator on 15/5/6.
//
//

#ifndef __AbelLLExplorer__Round025_01__
#define __AbelLLExplorer__Round025_01__

#include "RoundBase.h"

USING_NS_CC;

class Actor;
class StageScene;
class Round025_01 : public RoundBase
{
public:
    Round025_01();
    virtual ~Round025_01();
    
public:
    bool initWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    static Round025_01* createWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    
public:
    void cleanUp();
    virtual void onEnterRound();
    virtual void celebrate(const float);
    /* 开启触摸 */
    virtual void openTouch();
    /*  回合结束回调 */
    virtual void endRound();
    
    //渐隐出现灯塔
    void fadeInTowersSchedule(float);
    
    //渐隐消失灯塔
    void fadeOutTowersSchedule(float);
    
    /* 播放过关引导 */
    void playPassActor();
    
private:
    SpriteBatchNode* m_pBN = nullptr;
    Sprite* m_pTowers[3];               //塔
    Actor* m_pShipActor;                //船
    
    Vect m_towersPos[3];                //塔的坐标
    int m_InitAniIndex;

    int m_effectId = 0;                //音效play的id
    Actor* m_pBoguangActor = nullptr;
};


#endif /* defined(__AbelLLExplorer__Round025_01__) */
