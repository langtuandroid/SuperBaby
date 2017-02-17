//
//  Round021_01.h
//  AbelLLExplorer
//
//  Created by Administrator on 15/5/21.
//
//

#ifndef __AbelLLExplorer__Round021_01__
#define __AbelLLExplorer__Round021_01__

#include "RoundBase.h"

USING_NS_CC;

class Actor;
class StageScene;
class Round021_01 : public RoundBase
{
public:
    Round021_01();
    virtual ~Round021_01();
    
public:
    bool initWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    static Round021_01* createWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    
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
    
    //答对后改变actor动作
    void changeActor();
    
    //渐隐消失所有答案
    void fadeAll();
    
    //渐现桥
    void fadeOutBridge();
    
    //答对后改变绳子
    void changeRope();
    
private:
    Sprite* m_pAnswersSp[3];
    Vec2 m_answersPos[3];
    Sprite* m_pOverSp = nullptr;
    
    Sprite* m_pHintSp = nullptr;
    
    Sprite* m_pLightPillarSp = nullptr;
    
    Sprite* m_pBridgeSp = nullptr;
    
    Sprite* m_pRope;          //绳子
    Sprite* m_pPassRope;      //过关状态的绳子
    
    unsigned int m_InitAniIndex = 0;
    int m_effectId = 0;
    Actor* m_pMSActor = nullptr;
    bool m_opentouched = false;
    bool m_jumped = false;
};


#endif /* defined(__AbelLLExplorer__Round021_01__) */
