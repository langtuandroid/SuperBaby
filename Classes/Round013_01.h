//
//  Round013_01.h
//  AbelLLExplorer
//
//  Created by adinnet on 2015/3/6.
//
//  神秘星球 第十三关卡第一回合
//

#ifndef __AbelLLExplorer__Round013_01__
#define __AbelLLExplorer__Round013_01__

#include "RoundBase.h"

USING_NS_CC;

class Actor;
class StageScene;
class Round013_01 : public RoundBase
{
public:
    Round013_01();
    virtual ~Round013_01();

public:
    bool initWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    static Round013_01* createWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    
public:
    virtual void onEnterRound();
    virtual void celebrate(const float);
    /* 开启触摸 */
    virtual void openTouch();
    /*  回合结束回调 */
    virtual void endRound();
    
    /* 播放过关引导 */
    void playPassActor();
    
private:
    Vec2 m_originalPoints[3];
    Vec2 m_touchBeganPoint;         // 拖放开始位置

    // 三架飞机
    Actor* m_planeActors[3];
    
    // 三个基地
    Actor* m_baseActors[3];
    
    //
    int m_effectId = 0;                //音效play的id
    
    // 答对题的数量
    unsigned int m_numberOfdragComplete = 0;
};


#endif /* defined(__AbelLLExplorer__Round013_01__) */
