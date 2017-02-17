//
//  Round004_04.h
//  AbelLLExplorer
//
//  Created by Administrator on 15/3/27.
//
//

#ifndef __AbelLLExplorer__Round004_04__
#define __AbelLLExplorer__Round004_04__

#include "RoundBase.h"

USING_NS_CC;

class Actor;
class StageScene;
class Round004_04 : public RoundBase
{
public:
    Round004_04();
    virtual ~Round004_04();
    
public:
    bool initWithRoundIndex(const int,StageScene*, const Vec2& pos = Vec2::ZERO);
    static Round004_04* createWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    
public:
    virtual void celebrate(const float);
    /* 开启触摸 */
    virtual void openTouch();
    /*  回合结束回调 */
    virtual void endRound();
    
    //渐变显示乐器
    void fadeInInstrument();
    
    //渐变隐藏乐器
    void fadeOutInstrument();
    
    //答对后改变青蛙动作
    void changeFrog();
    
private:
    void cleanUp();
    virtual void onEnterRound();
private:
    Sprite* m_PINstruments[3];           //乐器组
    Actor* m_pFrog;                      //青蛙actor
    
    int m_effectId;
};

#endif /* defined(__AbelLLExplorer__Round004_04__) */
