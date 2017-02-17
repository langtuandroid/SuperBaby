//
//  Round043_04.h
//  superBaby
//
//  Created by Administrator on 15/7/31.
//
//

#ifndef __superBaby__Round043_04__
#define __superBaby__Round043_04__

#include "RoundBase.h"
#include <ui/CocosGUI.h>
USING_NS_CC;
class Actor;
class Round043_04 : public RoundBase
{
public:
    Round043_04();
    virtual ~Round043_04();
    
public:
    bool initWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    static Round043_04* createWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    
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
    
    //显示提示
    void showHintsSchedule(float);
    
    void changeActor();
    
    void openDoor();
    
    void moveLeadActor();
    
    void stageOver(float);
    
private:
    Sprite* m_pScrewSp = nullptr;
    Sprite* m_pDoorSp[2];
    Sprite* m_pShadowSp = nullptr;
    Sprite* m_pOverSp = nullptr;
    Sprite* m_pHintSp[2];
    Actor* m_pMSActor = nullptr;
    int m_effectId = 0;                //音效play的id
    int m_InitAniIndex = 0;
    GLubyte m_opacity = 0;
    LayerColor* m_LayerColor;
    bool m_opentouched = false;
    bool m_jumped = false;
};


#endif /* defined(__superBaby__Round043_04__) */
