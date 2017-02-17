//
//  Round024_02.h
//  AbelLLExplorer
//
//  Created by adinnet on 2015/3/9.
//
//  第 24 关卡第 2 回合
//

#ifndef __AbelLLExplorer__Round024_02__
#define __AbelLLExplorer__Round024_02__

#include "RoundBase.h"

USING_NS_CC;

class Actor;
class NpcActor;
class StageScene;
class Round024_02 : public RoundBase
{
public:
    Round024_02();
    virtual ~Round024_02();
    
public:
    bool initWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    static Round024_02* createWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    
public:
    virtual void onEnterRound();
    virtual void celebrate(const float);
    /* 开启触摸 */
    virtual void openTouch();
    /*  回合结束回调 */
    virtual void endRound();
    
    void fadeTopic();
    
private:
    /* 下降完成回调 */
    void downCompleteCB();
    
private:
    SpriteBatchNode* m_pSBN = nullptr;
    Sprite* m_numberSp[3];                  // 数字
    Sprite* m_jnumberSp[3];                 // 数字
    
    int m_numberOfDowned = 0;
    
    int m_effectId;                //音效play的id
};
#endif /* defined(__AbelLLExplorer__Round024_02__) */
