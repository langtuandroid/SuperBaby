//
//  Round041_04.h
//  superBaby
//
//  Created by Administrator on 15/7/28.
//
//

#ifndef __superBaby__Round041_04__
#define __superBaby__Round041_04__

#include "RoundBase.h"
#include <ui/CocosGUI.h>
USING_NS_CC;

class Actor;
class StageScene;
class Round041_04 : public RoundBase
{
public:
    Round041_04();
    virtual ~Round041_04();
    
public:
    bool initWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    static Round041_04* createWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    
public:
    void cleanUp();
    virtual void onEnterRound();
    virtual void celebrate(const float);
    /* 开启触摸 */
    virtual void openTouch();
    /*  回合结束回调 */
    virtual void endRound();
    
    //上升答案
    void upAnswersSchedule(float);
    
    void changeScaleActorSchedule(float);
        
    void fadeOutAnswers();
    
    void changeActor();
    
private:
    Sprite* m_pAnswersSp[3];
    Vec2 m_answersPos[3];
    Actor* m_pMSsActor[5];
    Vec2 m_mssPos[5];
    int m_effectId = 0;                 //音效play的id
    int m_InitAniIndex = 0;
    bool m_opentouched = false;
    bool m_jumped = false;
};


#endif /* defined(__superBaby__Round041_04__) */
