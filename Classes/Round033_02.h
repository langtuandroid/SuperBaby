//
//  Round033_02.h
//  AbelLLExplorer
//
//  Created by Administrator on 15/6/10.
//
//

#ifndef __AbelLLExplorer__Round033_02__
#define __AbelLLExplorer__Round033_02__

#include "RoundBase.h"

USING_NS_CC;

class Actor;
class StageScene;
class Round033_02 : public RoundBase
{
public:
    Round033_02();
    virtual ~Round033_02();
    
public:
    bool initWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    static Round033_02* createWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    
public:
    void cleanUp();
    virtual void onEnterRound();
    virtual void celebrate(const float);
    /* 开启触摸 */
    virtual void openTouch();
    /*  回合结束回调 */
    virtual void endRound();
    //改变答案大小以提示
    void changeScaleAnswersSchedule(float);
    //改变题目大小以提示
    void changeScaleTopicsSchedule(float);
        
private:
    Sprite* m_pAnswersSp[2];        //船
    Vec2 m_answersPos[2];
    Vec2 m_answersEndPos[2];
    Sprite* m_pTopicsSp[6];
    Vec2 m_topicsPos[6];
    int m_effectId = 0;                //音效play的id
    int m_InitAniIndex;
    bool m_opentouched = false;
    bool m_jumped = false;
    Actor* m_pPaopaoActor = nullptr;
    Actor* m_pWhiteLightActor = nullptr;
    unsigned int m_curCorrectNum = 0;
    Vec2 m_originalPoint;
    Vec2 m_touchBeganPoint;         // 拖放开始位置
};


#endif /* defined(__AbelLLExplorer__Round033_02__) */
