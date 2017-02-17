//
//  Round030_05.h
//  AbelLLExplorer
//
//  Created by Administrator on 15/6/8.
//
//

#ifndef __AbelLLExplorer__Round030_05__
#define __AbelLLExplorer__Round030_05__

#include "RoundBase.h"
USING_NS_CC;
class Actor;
class Round030_05 : public RoundBase
{
public:
    Round030_05();
    virtual ~Round030_05();
    
public:
    bool initWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    static Round030_05* createWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    
public:
    void cleanUp();
    virtual void onEnterRound();
    virtual void celebrate(const float);
    virtual void onPreLoadedRes();
    /* 开启触摸 */
    virtual void openTouch();
    /*  回合结束回调 */
    virtual void endRound();
    
    //放大答案以提示
    void changeScaleHint();
    
    //分享回调
    void shareCallback();
    
    //回合结束分享调用
    void roundEndShareCallBack(bool isShare);
    
    void changeScaleAnswersSchedule(float);
    
    //移动主角退出
    void moveActor();
    //渐变变黑退出
    void fadeOutLayer(float);
    
    //答对改变ms
    void changeActor();
    
    //隐藏剩余答案
    void fadeAnswers();
private:
    Sprite* m_pAnswersSp[3];            //答案图片
    Vec2 m_answersPos[3];               //答案坐标
    Actor* m_pMSActor = nullptr;        //ms
    Sprite* m_pBoardSp = nullptr;
    Sprite* m_pTopicSp = nullptr;
    Sprite* m_pHintTopicSp = nullptr;
    int m_effectId = 0;                //音效play的id
    int m_InitAniIndex;
    
    GLubyte m_opacity = 0;
    LayerColor* m_LayerColor;
    bool m_opentouched = false;
    bool m_jumped = false;
    Actor* m_pPaopaoActor = nullptr;
    Actor* m_pWhiteLightActor = nullptr;
};


#endif /* defined(__AbelLLExplorer__Round030_05__) */
