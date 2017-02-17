//
//  Round003_01.h
//  AbelLLExplorer
//
//  Created by adinnet on 2015/3/6.
//
//  第三关卡第一回合
//

#ifndef __AbelLLExplorer__Round003_01__
#define __AbelLLExplorer__Round003_01__

#include "RoundBase.h"

USING_NS_CC;

class Actor;
class StageScene;
class Round003_01 : public RoundBase
{
public:
    Round003_01();
    virtual ~Round003_01();

public:
    bool initWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    static Round003_01* createWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    
public:
    virtual void onEnterRound();
};

#endif /* defined(__AbelLLExplorer__Round003_01__) */
