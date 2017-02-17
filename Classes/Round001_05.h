//
//  Round001_05.h
//  AbelLLExplorer
//
//  Created by adinnet on 2015/3/9.
//
//

#ifndef __AbelLLExplorer__Round001_05__
#define __AbelLLExplorer__Round001_05__

#include "RoundBase.h"

USING_NS_CC;

class Actor;
class LeadingActor;
class StageScene;
class Round001_05 : public RoundBase
{
public:
    Round001_05();
    virtual ~Round001_05();
    
public:
    bool initWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    static Round001_05* createWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    
public:
    virtual void onEnterRound();
    virtual void celebrate(const float);
    virtual void onPreLoadedRes();
    /* 开启触摸 */
    virtual void openTouch();
    /*  回合结束回调 */
    virtual void endRound();
    
    /* 树和鸟上升*/
    void upTreeAndBirds();
    
    //鸟不同步眨眼
    void birdBlinkAction();
    
    void birdPlayAnimation();
    
    //选择正确后鸟的动作
    void chooseTrueActionBird();
    
    // 鸟飞走
    void birdFlyAction(int);
    
    /* 播放过关引导 */
    void playPassActor();
    
private:
    Node* m_pBirdTreeNode = nullptr;    // 鸟所在节点
    Actor* m_pBirdActors[5];            // 存放所有鸟 actor
    Vec2 m_birdInitPos[5];              // 鸟初始位置
    Rect m_AllRect[5];                  // 所有鸟区域
    int m_CurBirdIndex = 0;             // 当前所在鸟index
    int m_DelayTimeBirdIndex = 0;       // 延时开始鸟眨眼动画使用，当前鸟index
    Vec2 m_MovePos[5];                  // 鸟飞坐标点
    int m_effectId = 0;                 // 音效play的id
    int m_trueAnswerIndex = -1;         // 正确答案的index（打乱顺序后）
    bool m_opentouched = false;
    bool m_jumped = false;
};

#endif /* defined(__AbelLLExplorer__Round001_05__) */
