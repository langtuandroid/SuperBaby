//
//  Round032_04.h
//  AbelLLExplorer
//
//  Created by Administrator on 15/6/9.
//
//

#ifndef __AbelLLExplorer__Round032_04__
#define __AbelLLExplorer__Round032_04__

#include "RoundBase.h"
#include <ui/CocosGUI.h>
USING_NS_CC;
using namespace ui;
class Actor;
class Round032_04 : public RoundBase
{
public:
    Round032_04();
    virtual ~Round032_04();
    
public:
    bool initWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    static Round032_04* createWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    
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
    
    //上升答案
    void upAnswersSchedule(float);
    
    void downAnswersSchedule(float);
private:
    Sprite* m_pAnswersSp[3];  //答案图片
    Vec2 m_answersPos[3];               //答案坐标
    Vec2 m_answersEndPos[3];
    Actor* m_pMSActor = nullptr;
    int m_effectId = 0;                //音效play的id
    int m_InitAniIndex = 0;
    // 拖放答题成功的数量
    Vec2 m_originalPoint;
    Vec2 m_touchBeganPoint;         // 拖放开始位置
    Rect m_touchRect;
    GLubyte m_opacity = 0;
    LayerColor* m_LayerColor;
    bool m_opentouched = false;
    bool m_jumped = false;
    Actor* m_pPaopaoActor = nullptr;
    Actor* m_pWhiteLightActor = nullptr;
};

#endif /* defined(__AbelLLExplorer__Round032_04__) */
