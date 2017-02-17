//
//  Round002_02.h
//  AbelLLExplorer
//
//  Created by adinnet on 2015/3/9.
//
//  第二个关卡第一回合
//

#ifndef __AbelLLExplorer__Round002_02__
#define __AbelLLExplorer__Round002_02__

#include "RoundBase.h"

USING_NS_CC;

class Actor;
class StageScene;
class Round002_02 : public RoundBase
{
public:
    Round002_02();
    virtual ~Round002_02();
    
public:
    bool initWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    static Round002_02* createWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);

public:
    virtual void celebrate(const float);
    /* 开启触摸 */
    virtual void openTouch();
    /*  回合结束回调 */
    virtual void endRound();
    virtual void onPreLoadedRes();

    virtual void onEnterRound();
    
    //提示
    void changeScaleAnswersSchedule(float);
    
private:
    SpriteBatchNode* m_pBN = nullptr;                 //BN
    
    Actor* m_pFlowerActor = nullptr;      // 七彩花
    
    Sprite* m_sps[3];
    Sprite* m_spScales[3];
    
    unsigned int m_correctNumber = 0;
    Vec2 m_originalPoints[3];
    
    // 七彩花已经跳跃次数
    unsigned int m_numberOfAlreadyJump = 0;
    int m_effectId = 0;
    int m_InitAniIndex = 0;
    bool m_opentouched = false;
    bool m_jumped = false;
};
#endif /* defined(__AbelLLExplorer__Round002_02__) */
