//
//  Round042_01.h
//  superBaby
//
//  Created by Administrator on 15/7/30.
//
//

#ifndef __superBaby__Round042_01__
#define __superBaby__Round042_01__

#include "RoundBase.h"

USING_NS_CC;

class Actor;
class StageScene;
class Round042_01 : public RoundBase
{
public:
    Round042_01();
    virtual ~Round042_01();
    
public:
    bool initWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    static Round042_01* createWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    
public:
    virtual void onEnterRound();
};

#endif /* defined(__superBaby__Round042_01__) */