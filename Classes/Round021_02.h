//
//  Round021_02.h
//  AbelLLExplorer
//
//  Created by Administrator on 15/5/21.
//
//

#ifndef __AbelLLExplorer__Round021_02__
#define __AbelLLExplorer__Round021_02__

#include "RoundBase.h"

USING_NS_CC;

class Actor;
class StageScene;
class Round021_02 : public RoundBase
{
public:
    Round021_02();
    virtual ~Round021_02();
    
public:
    bool initWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    static Round021_02* createWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    
public:
    virtual void onEnterRound();
    virtual void celebrate(const float);
    /* 开启触摸 */
    virtual void openTouch();
    /*  回合结束回调 */
    virtual void endRound();
    
    //答案改变大小以提示
    void changeScaleAnswersSchedule(float);
    
    //题目改变大小以提示
    void changeScaleTopicsSchedule(float);
    
    //答对后渐隐all
    void fadeOutAll(float);
    
    //答对后改变actor动作
    void changeActor();
    
private:
    Sprite* m_pAnswersSp[3];
    Vec2 m_answersPos[3];
    Sprite* m_pTopicsSp[2];
    Sprite* m_pBridgeSp = nullptr;
    unsigned int m_InitAniIndex = 0;
    int m_effectId = 0;
    unsigned int m_curCorrectNumber = 0;
    Actor* m_pMSActor = nullptr;
    bool m_opentouched = false;
    bool m_jumped = false;
};

#endif /* defined(__AbelLLExplorer__Round021_02__) */
