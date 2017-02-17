//
//  Round022_03.h
//  AbelLLExplorer
//
//  Created by Administrator on 15/5/22.
//
//

#ifndef __AbelLLExplorer__Round022_03__
#define __AbelLLExplorer__Round022_03__

#include "RoundBase.h"
USING_NS_CC;

class Actor;
class StageScene;
class Round022_03 : public RoundBase
{
public:
    Round022_03();
    virtual ~Round022_03();
    
public:
    bool initWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    static Round022_03* createWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    
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
    void fadeAnswers();
    
    //答对后桥显示
    void fadeBridge();
    
private:
    Sprite* m_pAnswersSp[3];            //答案
    Vec2 m_answersPos[3];
    Sprite* m_pBridgeSp = nullptr;
    Sprite* m_pStageSp = nullptr;
    Actor* m_pCakeActor = nullptr;
    
    int m_InitAniIndex;
    int m_effectId = 0;                //音效play的id
    bool m_opentouched = false;
    bool m_jumped = false;
};


#endif /* defined(__AbelLLExplorer__Round022_03__) */
