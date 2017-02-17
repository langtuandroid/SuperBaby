//
//  Round028_02.h
//  AbelLLExplorer
//
//  Created by Administrator on 15/6/4.
//
//

#ifndef __AbelLLExplorer__Round028_02__
#define __AbelLLExplorer__Round028_02__

#include "RoundBase.h"

USING_NS_CC;

class Actor;
class StageScene;
class Round028_02 : public RoundBase
{
public:
    Round028_02();
    virtual ~Round028_02();
    
public:
    bool initWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    static Round028_02* createWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    
public:
    void cleanUp();
    virtual void onEnterRound();
    virtual void celebrate(const float);
    /* 开启触摸 */
    virtual void openTouch();
    /*  回合结束回调 */
    virtual void endRound();
    
    //依次放大城堡以提示
    void changeScaleAnswersSchedule(float);
    //改变大小以提示
    void changeScaleHint();
    
    //答对隐藏答案
    void fadeAnswers();
private:
    Sprite* m_pAnswersSp[2];
    Vec2 m_answersPos[2];
    Vec2 m_answersEndPos[2];
    Sprite* m_pTopicsSp[5];
    Vec2 m_topicsPos[5];
    Sprite* m_pHintSp = nullptr;
    Actor* m_pMSActor = nullptr;
    int m_InitAniIndex = 0;
    Vec2 m_originalPoint;
    Vec2 m_touchBeganPoint;         // 拖放开始位置
    int m_effectId = 0;                //音效play的id
    bool m_opentouched = false;
    bool m_jumped = false;
    Actor* m_pPaopaoActor = nullptr;
    Actor* m_pWhiteLightActor = nullptr;
};

#endif /* defined(__AbelLLExplorer__Round028_02__) */
