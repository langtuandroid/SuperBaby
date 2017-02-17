//
//  Round034_03.h
//  superBaby
//
//  Created by Administrator on 15/6/23.
//
//

#ifndef __superBaby__Round034_03__
#define __superBaby__Round034_03__

#include "RoundBase.h"
#include <ui/CocosGUI.h>
USING_NS_CC;

class Actor;
class StageScene;
class Round034_03 : public RoundBase
{
public:
    Round034_03();
    virtual ~Round034_03();
    
public:
    bool initWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    static Round034_03* createWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    
public:
    void cleanUp();
    virtual void onEnterRound();
    virtual void celebrate(const float);
    /* 开启触摸 */
    virtual void openTouch();
    /*  回合结束回调 */
    virtual void endRound();
    
    //依次放大锚和绳子
    void changeScaleSchedule(float);
    
private:
    LayerColor* m_pNodes[3];
    Vec2 m_nodesPos[3];
    ui::ImageView* m_pAnchorsImageView[3];
    Vec2 m_anchorsPos[3];
    ui::ImageView* m_pRopesImageView[3];
    Vec2 m_ropesPos[3];
    int m_effectId = 0;                 //音效play的id
    int m_InitAniIndex = 0;
    bool m_opentouched = false;
    bool m_jumped = false;
    Actor* m_pPaopaoActor = nullptr;
    Actor* m_pWhiteLightActor = nullptr;
};


#endif /* defined(__superBaby__Round034_03__) */
