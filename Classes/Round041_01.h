//
//  Round041_01.h
//  superBaby
//
//  Created by Administrator on 15/7/28.
//
//

#ifndef __superBaby__Round041_01__
#define __superBaby__Round041_01__

#include "RoundBase.h"

USING_NS_CC;

class Actor;
class StageScene;
class Round041_01 : public RoundBase
{
public:
    Round041_01();
    virtual ~Round041_01();
    
public:
    bool initWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    static Round041_01* createWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    
public:
    virtual void onEnterRound();
};

#endif /* defined(__superBaby__Round041_01__) */
