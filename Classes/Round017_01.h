//
//  Round017_01.h
//  AbelLLExplorer
//
//  Created by Administrator on 15/5/18.
//
//

#ifndef __AbelLLExplorer__Round017_01__
#define __AbelLLExplorer__Round017_01__

#include "RoundBase.h"

USING_NS_CC;

class Actor;
class StageScene;
class Round017_01 : public RoundBase
{
public:
    Round017_01();
    virtual ~Round017_01();
    
public:
    bool initWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    static Round017_01* createWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    
public:
    
    
public:
    virtual void onEnterRound();
    virtual void celebrate(const float);
    /* 开启触摸 */
    virtual void openTouch();
    /*  回合结束回调 */
    virtual void endRound();
    
    //改变大小出现
    void changeScaleNumbersSchedule(float);
    
    //提示
    void fadeHint();
    
    //答对后改变actor动作
    void changeActor();
    
    //答对后隐藏答案
    void scaleChangeNumbers();
    
    //答对后移动砖填补
    void moveBrick();
    
private:
    
    Sprite* m_pAnswerNumbersSp[3];
    
    Vec2 m_answerNumbersPos[3];
    
    Sprite* m_pHintSp;
    
    Sprite* m_pBrickSp;
    
    unsigned int m_InitAniIndex = 0;
    int m_effectId = 0;
    Actor* m_pMSActor = nullptr;
    bool m_opentouched = false;
    bool m_jumped = false;
};

#endif /* defined(__AbelLLExplorer__Round017_01__) */
