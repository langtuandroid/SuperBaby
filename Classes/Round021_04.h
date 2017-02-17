//
//  Round021_04.h
//  AbelLLExplorer
//
//  Created by Administrator on 15/5/21.
//
//

#ifndef __AbelLLExplorer__Round021_04__
#define __AbelLLExplorer__Round021_04__

#include "RoundBase.h"

USING_NS_CC;

class Actor;
class StageScene;
class Round021_04 : public RoundBase
{
public:
    Round021_04();
    virtual ~Round021_04();
    
public:
    bool initWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    static Round021_04* createWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    
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
    
    //放大对话框
    void changeScaleMessageBox();
    
    //改变尺寸以提示
    void scaleChangeAnswersSchedule(float);

    //答对后打开门
    void openDoor();
    
    //答对后消失对话框
    void fadeMessageBox(float);
    
private:
    
    Sprite* m_pMessageBoxSp = nullptr;
    
    Sprite* m_pAnswersSp[2];
    Vec2 m_answersPos[2];
    Sprite* m_pDoorSp = nullptr;
    Sprite* m_pDoorFrameSp = nullptr;
    Sprite* m_pUpWallSp = nullptr;
    Sprite* m_pRightWallSp = nullptr;
    
    int m_InitAniIndex;
    int m_effectId = 0;                //音效play的id
    GLubyte m_opacity = 0;
    LayerColor* m_LayerColor;
    bool m_opentouched = false;
    bool m_jumped = false;
};

#endif /* defined(__AbelLLExplorer__Round021_04__) */
