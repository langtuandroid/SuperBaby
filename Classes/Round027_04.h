//
//  Round027_04.h
//  AbelLLExplorer
//
//  Created by Administrator on 15/6/2.
//
//

#ifndef __AbelLLExplorer__Round027_04__
#define __AbelLLExplorer__Round027_04__

#include "RoundBase.h"

USING_NS_CC;
class Actor;
class Round027_04 : public RoundBase
{
public:
    Round027_04();
    virtual ~Round027_04();
    
public:
    bool initWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    static Round027_04* createWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    
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
    
    //移动海龟
    void moveMS();
    
    void changeScaleTopicSchedule(float);
private:
    Sprite* m_pAnswersSp[2];            //答案图片
    Vec2 m_answersPos[2];               //答案坐标
    Vec2 m_answersEndPos[2];
    Sprite* m_pTopicSp[7];
    Vec2 m_topicsPos[7];
    Sprite* m_pHintSp = nullptr;
    Actor* m_pMSActor = nullptr;        //ms
    Actor* m_pMermaidActor = nullptr;
    int m_effectId = 0;                //音效play的id
    int m_InitAniIndex;
    // 拖放答题成功的数量
    Vec2 m_originalPoint;
    Vec2 m_touchBeganPoint;         // 拖放开始位置
    
    GLubyte m_opacity = 0;
    LayerColor* m_LayerColor;
    bool m_opentouched = false;
    bool m_jumped = false;
    Actor* m_pBoguangActor = nullptr;
    Actor* m_pShipActor = nullptr;
    unsigned int m_randNumber;
};

#endif /* defined(__AbelLLExplorer__Round027_04__) */
