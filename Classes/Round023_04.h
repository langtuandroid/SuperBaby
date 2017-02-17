//
//  Round023_04.h
//  AbelLLExplorer
//
//  Created by Administrator on 2015/03/27.
//
//

#ifndef __AbelLLExplorer__Round023_04__
#define __AbelLLExplorer__Round023_04__

#include "RoundBase.h"

USING_NS_CC;

class Actor;
class SocialShareNode;
class StageScene;
class Round023_04 : public RoundBase
{
public:
    Round023_04();
    virtual ~Round023_04();
    
public:
    bool initWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    static Round023_04* createWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    
public:
    void cleanUp();
    virtual void onEnterRound();
    virtual void celebrate(const float);
    virtual void onPreLoadedRes();
    /* 开启触摸 */
    virtual void openTouch();
    /*  回合结束回调 */
    virtual void endRound();
    
    //回合结束分享调用
    void roundEndShareCallBack(bool isShare);
    
    //分享回调
    void shareCallback();
    
    //渐变变黑退出
    void fadeOutLayer(float);
    
private:
    void upCompleteCB();
    /* 答对回调 */
    void answerCorrectCB();
    
    /**/
    void openDoor();
    
private:
    SpriteBatchNode* m_pSBN = nullptr;
    
    Actor* m_pNpcActor = nullptr;               // npc actor
    Vec2 m_originalPoints[3];
    Vec2 m_touchBeganPoint;                     // 拖放开始位置
    
    Sprite* m_npcSp[3];                         // 形状 npc
    Sprite* m_jnpcSp[3];
    
    Sprite* m_pDoorSp = nullptr;
    
    // 门遮罩
    Sprite* m_doorMaskSp = nullptr;
    
    int m_effectId = 0;                         // 音效play的id
    GLubyte m_opacity = 0;
    LayerColor* m_LayerColor = nullptr;
    
    Vec2 m_npcTargetPos = Vec2(928.0f, 606.0f);
    
    bool m_isopentouched = false;
    bool m_isjumped = false;
    int m_numberOfUped = 0;
    bool m_npcIsMoveOut = false;
};

#endif /* defined(__AbelLLExplorer__Round023_04__) */
