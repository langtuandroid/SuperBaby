//
//  Round019_03.h
//  AbelLLExplorer
//
//  Created by Administrator on 15/5/19.
//
//

#ifndef __AbelLLExplorer__Round019_03__
#define __AbelLLExplorer__Round019_03__

#include "RoundBase.h"
USING_NS_CC;

class Actor;
class StageScene;
class Round019_03 : public RoundBase
{
public:
    Round019_03();
    virtual ~Round019_03();
    
public:
    bool initWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    static Round019_03* createWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    
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
    void changeScaleFadeSchedule(float);
    
private:
    Sprite* m_pAnswersSp[4];            //答案
    Vec2 m_answersPos[4];
    Actor* m_pMSActor = nullptr;
    
    int m_InitAniIndex;
    int m_effectId = 0;                //音效play的id
    bool m_opentouched = false;
    bool m_jumped = false;
};

#endif /* defined(__AbelLLExplorer__Round019_03__) */
