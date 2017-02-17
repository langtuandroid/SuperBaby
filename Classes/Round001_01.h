//
//  Round001_01.h
//  AbelLLExplorer
//
//  Created by adinnet on 15/3/6.
//
//  第一个关卡第一回合
//

#ifndef __AbelLLExplorer__Round001_01__
#define __AbelLLExplorer__Round001_01__

#include "RoundBase.h"

USING_NS_CC;

class Actor;
class StageScene;
class Round001_01 : public RoundBase
{
public:
    Round001_01();
    virtual ~Round001_01();

public:
    bool initWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    static Round001_01* createWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    
public:
    virtual void onEnterRound();
    virtual void celebrate(const float);
    virtual void onPreLoadedRes();
    /* 开启触摸 */
    virtual void openTouch();
    /*  回合结束回调 */
    virtual void endRound();
    
    // 回合开场动画，数字下坠动画调度
    void numberDonwSchedule(float);
    void scaleComplete();
    // 回合开场动画，小数字上升动画调度
    void numberUpSchedule(float);

    /* 淡出所有数字*/
    void fadeOutNumber();
    
    /* 播放过关引导 */
    void playPassActor();
    
private:
    Actor *m_pWheelActor = nullptr; // 数字转盘
    
    Vec2 m_answersPos[5];
    Vec2 m_dragBegin = Vec2::ZERO;

    // 拖放答题成功的数量
    unsigned int m_numberOfdragComplete = 0;
    
    SpriteBatchNode* m_pNumberBN = nullptr;
    Sprite* m_pAnswerNumbersSp[9];  // 答案数字
    Sprite* m_pNumberPadSp[5];      //
    Sprite* m_pNumberPadSpUp[5];    //
    
    unsigned int m_numbers[5] = {2, 3, 5, 7, 8};
    
    Vec2 m_numberInitPos[9];
    unsigned int m_InitAniIndex = 0;

    Sprite* m_pQuestionMark[2];     // 问号
    
    // 
    unsigned int m_result[2] = {0, 0};
    
    unsigned int m_downed = 0;
    int m_effectId = 0;                //音效play的id
    
};


#endif /* defined(__AbelLLExplorer__Round001_01__) */
