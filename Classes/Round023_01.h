//
//  Round023_01.h
//  AbelLLExplorer
//
//  Created by adinnet on 2015/03/06.
//
//  神秘星球第23关第1回合
//

#ifndef __AbelLLExplorer__Round023_01__
#define __AbelLLExplorer__Round023_01__

#include "RoundBase.h"

USING_NS_CC;

class Actor;
class StageScene;
class Round023_01 : public RoundBase
{
public:
    Round023_01();
    virtual ~Round023_01();

public:
    bool initWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    static Round023_01* createWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    
public:
    virtual void onEnterRound();
    virtual void celebrate(const float);
    /* 开启触摸 */
    virtual void openTouch();
    /*  回合结束回调 */
    virtual void endRound();

private:
    void jumpCompleted();
    
private:
    Actor* m_pActors[3];
    Actor* m_pJumpActors[3];
    
    //
    int m_effectId = 0;                //音效play的id
    
    int m_numberOfJumped = 0;
    long m_correctIndex = 2;
    bool m_opentouched = false;
    bool m_jumped = false;
};

#endif /* defined(__AbelLLExplorer__Round023_01__) */
