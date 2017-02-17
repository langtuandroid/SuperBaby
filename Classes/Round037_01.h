//
//  Round037_01.h
//  AbelLLExplorer
//
//  Created by Administrator on 15/5/8.
//
//

#ifndef __AbelLLExplorer__Round037_01__
#define __AbelLLExplorer__Round037_01__

#include "RoundBase.h"

USING_NS_CC;

class Actor;
class StageScene;
class Round037_01 : public RoundBase
{
public:
    Round037_01();
    virtual ~Round037_01();
    
public:
    bool initWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    static Round037_01* createWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    
public:
    void cleanUp();
    virtual void onEnterRound();
    virtual void celebrate(const float);
    /* 开启触摸 */
    virtual void openTouch();
    /*  回合结束回调 */
    virtual void endRound();
    
    /* 播放过关引导 */
    void playPassActor();
    //改变山的大小，以提示
    void scaleChangeMountSchedule(float);
    
private:
    SpriteBatchNode* m_pBN = nullptr;
    Sprite* m_pMounts[3];               //山
    
    Sprite* m_pBeforeBackGroundSp;     //前景
    
    Vect m_mountsPos[3];                //山的坐标
    int m_InitAniIndex;
    
    int m_effectId = 0;                //音效play的id
};


#endif /* defined(__AbelLLExplorer__Round037_01__) */
