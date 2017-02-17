//
//  Round019_04.h
//  AbelLLExplorer
//
//  Created by Administrator on 15/5/19.
//
//

#ifndef __AbelLLExplorer__Round019_04__
#define __AbelLLExplorer__Round019_04__

#include "RoundBase.h"

USING_NS_CC;

class Actor;
class StageScene;
class Round019_04 : public RoundBase
{
public:
    Round019_04();
    virtual ~Round019_04();
    
public:
    bool initWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    static Round019_04* createWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    
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
    
    //答对后依次消失题目
    void changeScaleGoodsSchedule(float);
    
private:
    Actor* m_pMSActor;                  //
    
    Sprite* m_pAnswersSp[5];            //答案
    Vec2 m_answersPos[5];
    Sprite* m_pCloundSp;
    
    int m_InitAniIndex;
    int m_effectId = 0;                //音效play的id
    GLubyte m_opacity = 0;
    LayerColor* m_LayerColor;
    bool m_opentouched = false;
    bool m_jumped = false;
};

#endif /* defined(__AbelLLExplorer__Round019_04__) */
