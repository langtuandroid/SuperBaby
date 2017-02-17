//
//  Round013_04.h
//  AbelLLExplorer
//
//  Created by Administrator on 2015/03/27.
//
//

#ifndef __AbelLLExplorer__Round013_04__
#define __AbelLLExplorer__Round013_04__

#include "RoundBase.h"

USING_NS_CC;

class Actor;
class SocialShareNode;
class StageScene;
class Round013_04 : public RoundBase
{
public:
    Round013_04();
    virtual ~Round013_04();
    
public:
    bool initWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    static Round013_04* createWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    
public:
    void cleanUp();
    virtual void onEnterRound();
    virtual void celebrate(const float);
    virtual void onPreLoadedRes();
    /* 开启触摸 */
    virtual void openTouch();
    /*  回合结束回调 */
    virtual void endRound();
    /* 处理 npc 收集 */
    virtual void processNpcCollect();
    
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
    /* */
    void moveCB();
    
private:
    Actor* m_pRobotActor[5];                    // 机器人 actor
    
    Vec2 m_originalPoints[5];
    Vec2 m_touchBeganPoint;         // 拖放开始位置
    
    Sprite* m_targetSp[3];
    bool m_targetSelected[3] = {false, false, false};
    
    Sprite* m_doorSp = nullptr;
    Sprite* m_OverSp = nullptr;
    
    unsigned int m_numberIndex = 0;
    
    unsigned int m_correctNumber = 0;           // 答对的数量
    
    int m_effectId = 0;                         // 音效play的id
    GLubyte m_opacity = 0;
    LayerColor* m_LayerColor = nullptr;
};

#endif /* defined(__AbelLLExplorer__Round013_04__) */
