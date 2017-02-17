//
//  Round041_05.h
//  superBaby
//
//  Created by Administrator on 15/7/28.
//
//

#ifndef __superBaby__Round041_05__
#define __superBaby__Round041_05__

#include "RoundBase.h"
USING_NS_CC;
class Actor;
class Round041_05 : public RoundBase
{
public:
    Round041_05();
    virtual ~Round041_05();
    
public:
    bool initWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    static Round041_05* createWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    
public:
    void cleanUp();
    virtual void onEnterRound();
    virtual void celebrate(const float);
    virtual void onPreLoadedRes();
    /* 开启触摸 */
    virtual void openTouch();
    /*  回合结束回调 */
    virtual void endRound();
    
    //分享回调
    void shareCallback();
    
    //回合结束分享调用
    void roundEndShareCallBack(bool isShare);
    
    //渐变变黑退出
    void fadeOutLayer(float);
    
    //提醒拖动目标区域
    void showHintSchedule(float);
    
    //放大答案
    void changeScaleAnswers();
    
    void changeScaleBridge();
    
    void upFadeOutAnswers();
        
    void moveLeadActor();
    
    void stageOver(float);
    
private:
    Sprite* m_pAnswersSp[3];  //答案图片
    Vec2 m_answersPos[3];               //答案坐标
    Vec2 m_answersEndPos[3];
    Sprite* m_pHintSp = nullptr;
    Sprite* m_pHintLightSp = nullptr;
    Sprite* m_pBridgeSp = nullptr;
    int m_effectId = 0;                //音效play的id
    int m_InitAniIndex = 0;
    GLubyte m_opacity = 0;
    LayerColor* m_LayerColor;
    bool m_opentouched = false;
    bool m_jumped = false;
    Vec2 m_originalPoint;
    Vec2 m_touchBeganPoint;         // 拖放开始位置
    
};

#endif /* defined(__superBaby__Round041_05__) */