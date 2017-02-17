//
//  Round047_04.h
//  superBaby
//
//  Created by Administrator on 15/8/5.
//
//

#ifndef __superBaby__Round047_04__
#define __superBaby__Round047_04__

#include "RoundBase.h"
USING_NS_CC;
class Actor;
class Round047_04 : public RoundBase
{
public:
    Round047_04();
    virtual ~Round047_04();
    
public:
    bool initWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    static Round047_04* createWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    
public:
    void cleanUp();
    virtual void onEnterRound();
    virtual void celebrate(const float);
    /* 开启触摸 */
    virtual void openTouch();
    /*  回合结束回调 */
    virtual void endRound();
    
    //分享回调
    void shareCallback();
    
    //回合结束分享调用
    void roundEndShareCallBack(bool isShare);
    
    //渐变变黑退出
    void fadeOutLayer(float);
    
    void changeScaleActorsSchedule(float);
    
    void changeActor();
    
    void moveActors();
    
    void moveLeadActor();
    
    void stageOver(float);
    
private:
    Actor* m_pMSActors[4];
    Vec2 m_msPos[4];
    int m_effectId = 0;                //音效play的id
    int m_InitAniIndex = 0;
    GLubyte m_opacity = 0;
    LayerColor* m_LayerColor;
    bool m_opentouched = false;
    bool m_jumped = false;
};


#endif /* defined(__superBaby__Round047_04__) */
