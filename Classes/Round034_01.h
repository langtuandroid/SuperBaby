//
//  Round034_01.h
//  superBaby
//
//  Created by Administrator on 15/6/23.
//
//

#ifndef __superBaby__Round034_01__
#define __superBaby__Round034_01__

#include "RoundBase.h"

USING_NS_CC;

class Actor;
class StageScene;
class Round034_01 : public RoundBase
{
public:
    Round034_01();
    virtual ~Round034_01();
    
public:
    bool initWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    static Round034_01* createWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    
public:
    virtual void onEnterRound();
    
private:

};

#endif /* defined(__superBaby__Round034_01__) */
