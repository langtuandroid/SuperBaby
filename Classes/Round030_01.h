//
//  Round030_01.h
//  AbelLLExplorer
//
//  Created by Administrator on 15/6/8.
//
//

#ifndef __AbelLLExplorer__Round030_01__
#define __AbelLLExplorer__Round030_01__

#include "RoundBase.h"

USING_NS_CC;

class Actor;
class StageScene;
class Round030_01 : public RoundBase
{
public:
    Round030_01();
    virtual ~Round030_01();
    
public:
    bool initWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    static Round030_01* createWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    
public:
    virtual void onEnterRound();
    
private:
    Sprite* m_pScrewSp = nullptr;
    Sprite* m_pDoorSp = nullptr;
    Sprite* m_pShadowSp = nullptr;
    Sprite* m_pOverSp = nullptr;
};

#endif /* defined(__AbelLLExplorer__Round030_01__) */
