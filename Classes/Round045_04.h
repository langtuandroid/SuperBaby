//
//  Round045_04.h
//  superBaby
//
//  Created by Administrator on 15/8/3.
//
//

#ifndef __superBaby__Round045_04__
#define __superBaby__Round045_04__

#include "RoundBase.h"
USING_NS_CC;
class Actor;
class Round045_04 : public RoundBase
{
public:
    Round045_04();
    virtual ~Round045_04();
    
public:
    bool initWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    static Round045_04* createWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    
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
    
    //改变答案大小
    void changeScaleAnswersSchedule(float);
    //移动主角退出
    void moveActor();

    void changeActor();
    
    void moveLeadActor();
    
    void openDoor();
    
private:
    Sprite* m_pAnswersSp[5];
    Vec2 m_answersPos[5];
    Vec2 m_answersEndPos[5];
    Actor* m_pMSActor = nullptr;
    Sprite* m_pGoodSp = nullptr;
    int m_effectId = 0;                //音效play的id
    int m_InitAniIndex = 0;
    GLubyte m_opacity = 0;
    LayerColor* m_LayerColor;
    bool m_opentouched = false;
    bool m_jumped = false;
    Vec2 m_originalPoint;
    Vec2 m_touchBeganPoint;         // 拖放开始位置
    Sprite* m_pDoorSp = nullptr;
    Sprite* m_pOverSp = nullptr;
};


#endif /* defined(__superBaby__Round045_04__) */
