//
//  Round012_01.h
//  AbelLLExplorer
//
//  Created by Administrator on 2015/4/9.
//
//

#ifndef __AbelLLExplorer__Round012_01__
#define __AbelLLExplorer__Round012_01__

#include "RoundBase.h"
USING_NS_CC;

class Actor;
class StageScene;
class Round012_01 : public RoundBase
{
public:
    Round012_01();
    virtual ~Round012_01();
    
public:
    bool initWithRoundIndex(const int,StageScene*, const Vec2& pos = Vec2::ZERO);
    static Round012_01* createWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    
public:
    virtual void celebrate(const float);
    virtual void onEnterRound();
    virtual void onPreLoadedRes();
    virtual void openTouch();
    /*  回合结束回调 */
    virtual void endRound();

private:
    
private:
    SpriteBatchNode* m_pBN;
    
    Sprite* m_pNumberSp[3];
    int m_effectId;
    bool m_opentouched = false;
    bool m_jumped = false;
};

#endif /* defined(__AbelLLExplorer__Round012_01__) */
