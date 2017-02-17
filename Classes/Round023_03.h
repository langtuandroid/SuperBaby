//
//  Round023_03.h
//  AbelLLExplorer
//
//  Created by adinnet on 2015/3/9.
//
//

#ifndef __AbelLLExplorer__Round023_03__
#define __AbelLLExplorer__Round023_03__

#include "RoundBase.h"

USING_NS_CC;

class Actor;
class LeadingActor;
class StageScene;
class Round023_03 : public RoundBase
{
public:
    Round023_03();
    virtual ~Round023_03();
    
public:
    bool initWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    static Round023_03* createWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    
public:
    virtual void onEnterRound();
    virtual void celebrate(const float);
    virtual void onPreLoadedRes();
    /* 开启触摸 */
    virtual void openTouch();
    /*  回合结束回调 */
    virtual void endRound();
    
    void fadeBasketAndGrapes(float);

private:
    void rotateGrapeSp(const long);
    void placeGrapeSpInBasket(const int, Sprite*);
    
private:
    SpriteBatchNode* m_pSBN = nullptr;
    Actor* m_pNpcActor = nullptr;
    
    Sprite* m_pGrapeSps[5];
    Vec2 m_grapePos[5];
    
    Vec2 m_originalPoints[5];
    Vec2 m_touchBeganPoint;                 // 拖放开始位置
    
    Sprite* m_basketSp = nullptr;
    
    int m_numberOfCorrect = 0;
    int m_effectId = 0;                //音效play的id
};

#endif /* defined(__AbelLLExplorer__Round023_03__) */
