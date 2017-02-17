//
//  Round009_04.h
//  AbelLLExplorer
//
//  Created by Administrator on 2015/4/3.
//
//

#ifndef __AbelLLExplorer__Round009_04__
#define __AbelLLExplorer__Round009_04__

#include "RoundBase.h"
USING_NS_CC;

class Actor;
class SocialShareNode;
class StageScene;
class Round009_04 : public RoundBase
{
public:
    Round009_04();
    virtual ~Round009_04();
    
public:
    bool initWithRoundIndex(const int,StageScene*, const Vec2& pos = Vec2::ZERO);
    static Round009_04* createWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    
public:
    virtual void celebrate(const float);
    virtual void onEnterRound();
    virtual void onPreLoadedRes();
    virtual void openTouch();
    /*  回合结束回调 */
    virtual void endRound();

    //回合结束分享调用
    void roundEndShareCallBack(bool isShare);
    //分享回调
    void shareCallback();
    
    //渐变变黑退出
    void fadeOutLayer(float);
    
    //依次放大
    void changeScaleSchedule(float);
    
private:
    void cleanUp();
    void addListener();
    void moveActor();
    void moveActorComplete(float);
    
private:
    SpriteBatchNode* m_pBSN;
    SocialShareNode* m_pSSNode;         // 社交分享

    Sprite* m_pSugarSp[3];
    Actor* m_pCrowGaf;
    
    Vec2 m_originalPoint;
    Vec2 m_touchBeganPoint;
    int m_correctNumber;
    int m_effectId;
    unsigned int m_InitAniIndex = 0;
    
    GLubyte m_opacity = 0;
    LayerColor* m_LayerColor;
    bool m_opentouched = false;
    bool m_jumped = false;
};

#endif /* defined(__AbelLLExplorer__Round009_04__) */
