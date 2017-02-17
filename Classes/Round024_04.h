//
//  Round024_04.h
//  AbelLLExplorer
//
//  Created by Administrator on 2015/03/27.
//
//

#ifndef __AbelLLExplorer__Round024_04__
#define __AbelLLExplorer__Round024_04__

#include "RoundBase.h"

USING_NS_CC;

class Actor;
class SocialShareNode;
class StageScene;
class Round024_04 : public RoundBase
{
public:
    Round024_04();
    virtual ~Round024_04();
    
public:
    bool initWithRoundIndex(const int, StageScene*, const Vec2&, const Vec2& pos = Vec2::ZERO);
    static Round024_04* createWithRoundIndex(const int, StageScene*, const Vec2&, const Vec2& pos = Vec2::ZERO);
    
public:
    void cleanUp();
    virtual void onEnterRound();
    virtual void celebrate(const float);
    virtual void onPreLoadedRes();
    /* 开启触摸 */
    virtual void openTouch();
    /*  回合结束回调 */
    virtual void endRound();
    
    //主角移动完成
    void moveActorComplete(float);
    
    //回合结束分享调用
    void roundEndShareCallBack(bool isShare);
    
    //分享回调
    void shareCallback();
    
    //渐变变黑退出
    void fadeOutLayer(float);
    
private:
    /* 缩放完成回调 */
    void scaleCompleteCB();
    /* 答对回调 */
    void answerCorrectCB();
    /* 是否满格的电源 */
    bool isPowerFullBattery(const long);
    /* 登上火箭 */
    void getOnRocket();
    /* 火箭上升后 */
    void rocketUped();
    
    //答对后隐藏剩余电池
    void fadeBattery(float);
    //收集主题BS
    void addBSActor();
    
private:
    SpriteBatchNode* m_pSBN = nullptr;
    Actor* m_pRocketActor = nullptr;            // 火箭 actor
    Actor* m_pBSActor = nullptr;
    Vec2 m_originalPoints[5];
    Vec2 m_touchBeganPoint;                     // 拖放开始位置
    
    Sprite* m_pBatterySp[5];                    // 电池
    Sprite* m_pjBatterySp[5];                   //
    
    unsigned int m_numberOfDraged = 0;          // 已经拖放的数量
    
    unsigned int m_correctNumber = 0;
    
    Rect m_targerRect[4];
    bool m_placed[4] = {false, false, false, false};
    
    int m_effectId = 0;                         // 音效play的id
    GLubyte m_opacity = 0;
    LayerColor* m_LayerColor = nullptr;
    
    bool m_scaled = false;
    bool m_opentouched = false;
    int m_nScaleIndex = 0;
    int m_numberOfPlaced = 0;
};

#endif /* defined(__AbelLLExplorer__Round024_04__) *///


