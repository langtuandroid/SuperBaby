//
//  Round010_04.h
//  AbelLLExplorer
//
//  Created by Administrator on 15/4/8.
//
//

#ifndef __AbelLLExplorer__Round010_04__
#define __AbelLLExplorer__Round010_04__

#include "RoundBase.h"
USING_NS_CC;

class Actor;
class SocialShareNode;
class StageScene;
class Round010_04 : public RoundBase
{
public:
    Round010_04();
    virtual ~Round010_04();
    
public:
    bool initWithRoundIndex(const int,StageScene*, const Vec2& pos = Vec2::ZERO);
    static Round010_04* createWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    
public:
    virtual void celebrate(const float);
    virtual void onEnterRound();
    virtual void onPreLoadedRes();
    virtual void openTouch();
    /*  回合结束回调 */
    virtual void endRound();

    //移动主角退出
    void moveActor();
    
    //回合开始，手套上升
    void upGlove(float);
    
    //设置题目手套渐变显隐
    void gloveFadeSchedule(float);
    
    //回合结束，手套下降
    void downGlove();
    
    //回合结束，章鱼actor改变动作
    void changeOctopusActor();
    //回合结束分享调用
    void roundEndShareCallBack(bool isShare);
    
    //分享回调
    void shareCallback();
    
    //渐变变黑退出
    void fadeOutLayer(float);
    
private:
    void cleanUp();
    
private:
    
    SpriteBatchNode* m_pBN = nullptr;           //BN
    Sprite* m_pGlove[4];              //答案手套组
    Sprite* m_pTopicGloves[2];        //题目手套组
    Actor* m_pLeafActor = nullptr;             //荷叶actor
    Actor* m_pOctopusActor = nullptr;         //章鱼actor
    Actor* m_pWaveActor = nullptr;
    Sprite* m_pLeafSp = nullptr;
    Vec2 m_glovesPos[4];            //手套坐标组
    
    SocialShareNode* m_pSSNode;        // 社交分享
    
    Vec2 m_originalPoint;
    Vec2 m_touchBeganPoint;
    unsigned int m_InitAniIndex;
    int m_effectId;
    Vec2 m_originalPoints[4];
    
    GLubyte m_opacity = 0;
    LayerColor* m_LayerColor;
    bool m_opentouched = false;
    bool m_jumped = false;
};

#endif /* defined(__AbelLLExplorer__Round010_04__) */
