//
//  Round026_02.h
//  AbelLLExplorer
//
//  Created by Administrator on 15/6/1.
//
//

#ifndef __AbelLLExplorer__Round026_02__
#define __AbelLLExplorer__Round026_02__

#include "RoundBase.h"

USING_NS_CC;

class Actor;
class StageScene;
class Round026_02 : public RoundBase
{
public:
    Round026_02();
    virtual ~Round026_02();
    
public:
    bool initWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    static Round026_02* createWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    
public:
    void cleanUp();
    virtual void onEnterRound();
    virtual void celebrate(const float);
    /* 开启触摸 */
    virtual void openTouch();
    /*  回合结束回调 */
    virtual void endRound();
    //依次放大船以提示
    void changeScaleShipsSchedule(float);
    
private:
    SpriteBatchNode* m_pBN = nullptr;
    
    Actor* m_pShipsActor[3];        //船
    Vec2 m_shipsPos[3];
    
    int m_effectId = 0;                //音效play的id
    int m_InitAniIndex;
    bool m_opentouched = false;
    bool m_jumped = false;
    Actor* m_pBoguangActor = nullptr;

};

#endif /* defined(__AbelLLExplorer__Round026_02__) */
