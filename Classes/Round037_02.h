//
//  Round037_02.h
//  AbelLLExplorer
//
//  Created by Administrator on 15/5/8.
//
//

#ifndef __AbelLLExplorer__Round037_02__
#define __AbelLLExplorer__Round037_02__

#include "RoundBase.h"

USING_NS_CC;

class Actor;
class StageScene;
class Round037_02 : public RoundBase
{
public:
    Round037_02();
    virtual ~Round037_02();
    
public:
    bool initWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    static Round037_02* createWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    
public:
    void cleanUp();
    virtual void onEnterRound();
    virtual void celebrate(const float);
    /* 开启触摸 */
    virtual void openTouch();
    /*  回合结束回调 */
    virtual void endRound();
    //改变花的大小
    void scaleChangeFlowersSchedule(float);
    
    //答对后转动花，并播放特效
    void chooseTrueComplete(Node*);
    
private:
    SpriteBatchNode* m_pBN = nullptr;

    Sprite* m_pFlowers[4];
    
    Vec2 m_flowersPos[4];
    Actor* m_pEffectActor;

    int m_effectId = 0;                //音效play的id
    int m_InitAniIndex;
    
};

#endif /* defined(__AbelLLExplorer__Round037_02__) */
