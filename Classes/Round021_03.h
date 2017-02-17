//
//  Round021_03.h
//  AbelLLExplorer
//
//  Created by Administrator on 15/5/21.
//
//

#ifndef __AbelLLExplorer__Round021_03__
#define __AbelLLExplorer__Round021_03__

#include "RoundBase.h"
USING_NS_CC;

class Actor;
class StageScene;
class Round021_03 : public RoundBase
{
public:
    Round021_03();
    virtual ~Round021_03();
    
public:
    bool initWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    static Round021_03* createWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    
public:
    void cleanUp();
    virtual void onEnterRound();
    virtual void celebrate(const float);
    /* 开启触摸 */
    virtual void openTouch();
    /*  回合结束回调 */
    virtual void endRound();
    
    //依次放大答案，以提示
    void changeScaleAnswersSchedule(float);
    
    //答对后改变actor动作
    void changeActor();
    
    //答对后渐隐消失
    void fadeAnswers(float);
    
private:
    Sprite* m_pAnswersSp[2];            //答案
    Vec2 m_answersPos[3];
    Sprite* m_pBoardSp = nullptr;
    Actor* m_pAnswerActor = nullptr;
    Sprite* m_pOverSp= nullptr;
    Sprite* m_pLightPillarSp = nullptr;
    Sprite* m_pBridgeSp = nullptr;

    Rect m_answersRect[3];
    int m_InitAniIndex;
    int m_effectId = 0;                //音效play的id
    bool m_opentouched = false;
    bool m_jumped = false;
};


#endif /* defined(__AbelLLExplorer__Round021_03__) */
