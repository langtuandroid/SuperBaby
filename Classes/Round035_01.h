//
//  Round035_01.h
//  superBaby
//
//  Created by Administrator on 15/7/6.
//
//

#ifndef __superBaby__Round035_01__
#define __superBaby__Round035_01__

#include "RoundBase.h"

USING_NS_CC;

class Actor;
class StageScene;
class Round035_01 : public RoundBase
{
public:
    Round035_01();
    virtual ~Round035_01();
    
public:
    bool initWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    static Round035_01* createWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    
public:
    virtual void onEnterRound();
    
private:
    Sprite* m_pScrewSp = nullptr;
    Sprite* m_pDoorSp = nullptr;
    Sprite* m_pShadowSp = nullptr;
    Sprite* m_pOverSp = nullptr;
};

#endif /* defined(__superBaby__Round035_01__) */
