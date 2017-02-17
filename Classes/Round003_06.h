//
//  Round003_06.h
//  AbelLLExplorer
//
//  Created by Administrator on 15/4/24.
//
//

#ifndef __AbelLLExplorer__Round003_06__
#define __AbelLLExplorer__Round003_06__

#include "RoundBase.h"

USING_NS_CC;

class Actor;
class SocialShareNode;
class StageScene;
class Round003_06 : public RoundBase
{
public:
    Round003_06();
    virtual ~Round003_06();
    
public:
    bool initWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    static Round003_06* createWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    
public:
    virtual void onEnterRound();
    virtual void celebrate(const float);
    virtual void onPreLoadedRes();
    //渐隐消失骰子
    void fadeOutDices();
    //回合结束分享调用
    void roundEndShareCallBack(bool isShare);
    /* 开启触摸 */
    virtual void openTouch();
    /*  回合结束回调 */
    virtual void endRound();
    /*  处理npc收集 */
    virtual void processNpcCollect();
    
    //npc收集完成
    void npcCollectOver();
    
    //分享回调
    void shareCallback();
    //渐变变黑退出
    void fadeOutLayer(float);
    //渐现提示
    void fadeHint();
private:
    SpriteBatchNode* m_pSBN = nullptr;
    Sprite* m_pDiceSps[3];              // 骰子
    
    SocialShareNode* m_pSSNode;         /**/
    
    Actor* m_pActor;                    // 独眼怪
    
    Sprite* m_pHintSp = nullptr;
    
    Vec2 m_originalPoint;               // 原来的位置
    Vec2 m_touchBeganPoint;             // 拖放开始位置
    
    int m_effectId;
    Vec2 m_originalPoints[3];
    GLubyte m_opacity = 0;
    LayerColor* m_LayerColor;
    bool m_opentouched = false;
    bool m_jumped = false;
};

#endif /* defined(__AbelLLExplorer__Round003_06__) */
