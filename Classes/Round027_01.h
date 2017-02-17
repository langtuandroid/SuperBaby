//
//  Round027_01.h
//  AbelLLExplorer
//
//  Created by Administrator on 15/6/2.
//
//

#ifndef __AbelLLExplorer__Round027_01__
#define __AbelLLExplorer__Round027_01__

#include "RoundBase.h"

USING_NS_CC;

class Actor;
class StageScene;
class Round027_01 : public RoundBase
{
public:
    Round027_01();
    virtual ~Round027_01();
    
public:
    bool initWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    static Round027_01* createWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    
public:
    void cleanUp();
    virtual void onEnterRound();
    virtual void celebrate(const float);
    /* 开启触摸 */
    virtual void openTouch();
    /*  回合结束回调 */
    virtual void endRound();
    
    //依次放大城堡以提示
    void changeScaleGoodsSchedule(float);
    
    //答对隐藏答案
    void fadeGoods();
private:
    Sprite* m_pGoodsSp[3];
    Vec2 m_goodsPos[3];
    int m_InitAniIndex;
    
    int m_effectId = 0;                //音效play的id
    bool m_opentouched = false;
    bool m_jumped = false;
    Actor* m_pBoguangActor = nullptr;

};

#endif /* defined(__AbelLLExplorer__Round027_01__) */
