//
//  Round004_05.h
//  AbelLLExplorer
//
//  Created by Administrator on 15/4/24.
//
//

#ifndef __AbelLLExplorer__Round004_05__
#define __AbelLLExplorer__Round004_05__

#include "RoundBase.h"

USING_NS_CC;

class Actor;
class SocialShareNode;
class StageScene;
class Round004_05 : public RoundBase
{
public:
    Round004_05();
    virtual ~Round004_05();
    
public:
    bool initWithRoundIndex(const int,StageScene*, const Vec2& pos = Vec2::ZERO);
    static Round004_05* createWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    
public:
    virtual void celebrate(const float);
    /* 开启触摸 */
    virtual void openTouch();
    /*  回合结束回调 */
    virtual void endRound();
    
    //渐变显示树桩
    void fadeInTrees();
    //渐变隐藏树桩
    void fadeOutTrees();
    
    //改变猴子动作
    void changeMonkey();
    
    //移动主角退出
    void moveActor();
    
    //主角移动完成
    void moveActorComplete(float);
    //回合结束分享调用
    void roundEndShareCallBack(bool isShare);
    
    //分享回调
    void shareCallback();
    //渐变变黑退出
    void fadeOutLayer(float);
private:
    void cleanUp();
    virtual void onEnterRound();
private:
    Sprite* m_pTrees[3];                 //树桩组
    Actor* m_pMonkey;                    //猴子actor
    
    SocialShareNode* m_pSSNode;        // 社交分享
    int m_effectId;
    GLubyte m_opacity = 0;
    LayerColor* m_LayerColor;
};


#endif /* defined(__AbelLLExplorer__Round004_05__) */
