//
//  Round046_04.h
//  superBaby
//
//  Created by Administrator on 15/8/4.
//
//

#ifndef __superBaby__Round046_04__
#define __superBaby__Round046_04__

#include "RoundBase.h"
USING_NS_CC;
class Actor;
class Round046_04 : public RoundBase
{
public:
    Round046_04();
    virtual ~Round046_04();
    
public:
    bool initWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    static Round046_04* createWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    
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
    
    void changeActor(const unsigned int);
    
    void moveLeadActor();
    
    void moveMSActors();
        
private:
    Sprite* m_pAnswersSp[3];
    Vec2 m_answersPos[3];
    Vec2 m_answersEndPos[3];
    Vec2 m_answersCorPos[3];
    Actor* m_pMSActor[3];
    Vec2 m_msesPos[3];
    int m_effectId = 0;                //音效play的id
    int m_InitAniIndex = 0;
    GLubyte m_opacity = 0;
    LayerColor* m_LayerColor;
    bool m_opentouched = false;
    bool m_jumped = false;
    Vec2 m_originalPoint;
    Vec2 m_touchBeganPoint;         // 拖放开始位置
    unsigned int m_curCorrectNum = 0;
};


#endif /* defined(__superBaby__Round046_04__) */
