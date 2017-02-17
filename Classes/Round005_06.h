//
//  Round005_06.h
//  AbelLLExplorer
//
//  Created by Administrator on 15/4/27.
//
//

#ifndef __AbelLLExplorer__Round005_06__
#define __AbelLLExplorer__Round005_06__

#include "RoundBase.h"

USING_NS_CC;

class Actor;
class SocialShareNode;
class StageScene;
class Round005_06 : public RoundBase
{
public:
    Round005_06();
    virtual ~Round005_06();
    
public:
    bool initWithRoundIndex(const int,StageScene*, const Vec2& pos = Vec2::ZERO);
    static Round005_06* createWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    
public:
    virtual void celebrate(const float);
    /* 开启触摸 */
    virtual void openTouch();
    /*  回合结束回调 */
    virtual void endRound();
    
    //移动主角退出
    void moveActor();
    
    // 回合开场动画，小数字上升动画调度
    void numberUpSchedule(float);
    // 回合结束动画，小数字下降动画调度
    void numberDownSchedule(float);
    //积木渐变出现
    void fadeInToyblock();
    
    //积木渐变隐藏
    void fadeOutToyblock();
    
    //答对时改变猴子动作
    void changeMonkey();
    
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
    Sprite* m_pChooseNumber[3];       //选择的数字
    SocialShareNode* m_pSSNode;        // 社交分享
    unsigned int m_InitAniIndex;
    Sprite* m_PToyblock;               //积木
    Actor* m_pMonkey;                  //猴子
    
    int m_effectId;
    GLubyte m_opacity = 0;
    LayerColor* m_LayerColor;
    bool m_opentouched = false;
    bool m_jumped = false;
};

#endif /* defined(__AbelLLExplorer__Round005_06__) */
