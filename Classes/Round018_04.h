//
//  Round018_04.h
//  AbelLLExplorer
//
//  Created by Administrator on 15/5/18.
//
//

#ifndef __AbelLLExplorer__Round018_04__
#define __AbelLLExplorer__Round018_04__

#include "RoundBase.h"

USING_NS_CC;

class Actor;
class StageScene;
class Round018_04 : public RoundBase
{
public:
    Round018_04();
    virtual ~Round018_04();
    
public:
    bool initWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    static Round018_04* createWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    
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
    
    //移动主角退出
    void moveActor();
    //渐变变黑退出
    void fadeOutLayer(float);
    
    //改变尺寸以提示
    void scaleChangeGoodsSchedule(float);
    
    //改变actor
    void changeActor();
    
    //隐藏剩余答案
    void fadeOutAnswers();
    
    //答对后依次消失题目
    void changeScaleTopicsSchedule(float);
    
    //上移actor
    void upMsActor();
    
    //下移桥
    void downBridge();
    
private:
    Actor* m_pMSActor = nullptr;        //
    Actor* m_pHintActor = nullptr;
    
    Sprite* m_pAnswersSp[3];            // 答案
    Sprite* m_pTopicsSp[8];
    Sprite* m_pBridgeSp = nullptr;
    
    Vec2 m_answersPos[3];
    Vec2 m_answersEndPos[3];
    
    int m_InitAniIndex = 0;
    int m_effectId = 0;                     // 音效play的id
    GLubyte m_opacity = 0;
    LayerColor* m_LayerColor = nullptr;
    Vec2 m_originalPoint = Vec2::ZERO;
    Vec2 m_touchBeganPoint = Vec2::ZERO;    // 拖放开始位置
    bool m_opentouched = false;
    bool m_jumped = false;
};

#endif /* defined(__AbelLLExplorer__Round018_04__) */
