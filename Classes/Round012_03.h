//
//  Round012_03.h
//  AbelLLExplorer
//
//  Created by Administrator on 2015/4/9.
//
//

#ifndef __AbelLLExplorer__Round012_03__
#define __AbelLLExplorer__Round012_03__

#include "RoundBase.h"
USING_NS_CC;

class Actor;
class StageScene;
class Round012_03 : public RoundBase
{
public:
    Round012_03();
    virtual ~Round012_03();
    
public:
    bool initWithRoundIndex(const int,StageScene*, const Vec2& pos = Vec2::ZERO);
    static Round012_03* createWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    
public:
    virtual void celebrate(const float);
    virtual void onEnterRound();
    virtual void onPreLoadedRes();
    virtual void openTouch();
    /*  回合结束回调 */
    virtual void endRound();
    /*  处理npc收集 */
    virtual void processNpcCollect();

    // 回合开场动画，符号上升动画调度
    void markUpSchedule(float);
    
    // 回合开场动画，题目渐变出现，以及相关动画
    void fadeInActions();
    
    //答对改变翘翘板动作
    void changeActor();
    
    //答对后剩余答案渐隐
    void fadeOutAnswer();
    
    //添加npc图片
    void addNpcSp();

private:
    void cleanUp();
    
private:
    SpriteBatchNode* m_pBN;     //BN
    Sprite* m_pAnswers[2];      //答案组
    Sprite* m_pTopicBoard;      //题目板组
    Sprite* m_pAskMarks[2];     //问号组
    
    Vec2 m_answersPos[2];       //答案坐标组
    
    Actor* m_pTeetertotterActor;     //跷跷板actor
    
    Sprite* m_pNpcSp;

    Vec2 m_originalPoint;
    Vec2 m_touchBeganPoint;
    unsigned int m_InitAniIndex;
    int m_effectId;
    Vec2 m_originalPoints[2];
    bool m_opentouched = false;
    bool m_jumped = false;
};

#endif /* defined(__AbelLLExplorer__Round012_03__) */
