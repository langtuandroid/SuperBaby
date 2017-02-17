//
//  Round019_02.h
//  AbelLLExplorer
//
//  Created by Administrator on 15/5/19.
//
//

#ifndef __AbelLLExplorer__Round019_02__
#define __AbelLLExplorer__Round019_02__

#include "RoundBase.h"

USING_NS_CC;

class Actor;
class StageScene;
class Round019_02 : public RoundBase
{
public:
    Round019_02();
    virtual ~Round019_02();
    
public:
    bool initWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    static Round019_02* createWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    
public:
    virtual void onEnterRound();
    virtual void celebrate(const float);
    /* 开启触摸 */
    virtual void openTouch();
    /*  回合结束回调 */
    virtual void endRound();
    
    /*  处理npc收集 */
    virtual void processNpcCollect();
    
    //改变大小出现
    void changeScaleTopicsSchedule(float);
    
    void changeScaleAnswersSchedule(float);
    
    //突出问号以提示
    void scaleChangeMarksSchedule(float);
    
    //答对后渐隐all
    void fadeOutAll(float);
    
    //答对后改变actor动作
    void changeActor();
    
private:
    Sprite* m_pAnswerNumbersSp[3];
    Vec2 m_answerNumbersPos[3];
    Vec2 m_answerNumbersEndPos[3];
    Sprite* m_pTopicsSp[6];
    Sprite* m_pStageSp = nullptr;
    Actor* m_pLightActor = nullptr;
    
    Actor* m_pNpcActor = nullptr;
    
    unsigned int m_InitAniIndex = 0;
    int m_effectId = 0;
    unsigned int m_curCorrectNumber = 0;
    Vec2 m_originalPoint;
    Vec2 m_touchBeganPoint;         // 拖放开始位置
    bool m_opentouched = false;
    bool m_jumped = false;
};

#endif /* defined(__AbelLLExplorer__Round019_02__) */
