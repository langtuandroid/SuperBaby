//
//  Round033_04.h
//  AbelLLExplorer
//
//  Created by Administrator on 15/6/10.
//
//

#ifndef __AbelLLExplorer__Round033_04__
#define __AbelLLExplorer__Round033_04__

#include "RoundBase.h"
#include <ui/CocosGUI.h>
USING_NS_CC;
using namespace ui;
class Actor;
class Round033_04 : public RoundBase
{
public:
    Round033_04();
    virtual ~Round033_04();
    
public:
    bool initWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    static Round033_04* createWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    
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
    
    //放大答案
    void changeScaleSmallAnswersSchedule(float);
    
    void changeScaleClockSchedule(float);
    //缩小
    void changeScaleBigAnswersSchedule(float);
    
    void moveLeadActor();
    
    void stageOver(float);
    
private:
    Sprite* m_pAnswersSp[3];  //答案图片
    Vec2 m_answersPos[3];               //答案坐标
    Sprite* m_pClockSp = nullptr;       //钟
    Sprite* m_pStageSp = nullptr;       //平台
    int m_effectId = 0;                //音效play的id
    int m_InitAniIndex = 0;
    GLubyte m_opacity = 0;
    LayerColor* m_LayerColor;
    bool m_opentouched = false;
    bool m_jumped = false;
    Actor* m_pPaopaoActor = nullptr;
    Actor* m_pWhiteLightActor = nullptr;
};

#endif /* defined(__AbelLLExplorer__Round033_04__) */
