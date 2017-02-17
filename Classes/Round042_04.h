//
//  Round042_04.h
//  superBaby
//
//  Created by Administrator on 15/7/30.
//
//

#ifndef __superBaby__Round042_04__
#define __superBaby__Round042_04__

#include "RoundBase.h"
#include <ui/CocosGUI.h>
USING_NS_CC;

class Actor;
class StageScene;
class Round042_04 : public RoundBase
{
public:
    Round042_04();
    virtual ~Round042_04();
    
public:
    bool initWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    static Round042_04* createWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    
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
    
    void changeScaleApplesSchedule(float);
    
    void downAnswersSchedule(float);
    
private:
    Sprite* m_pAnswersSp[3];
    Vec2 m_answersPos[3];
    Sprite* m_pApplesSp[15];
    Vec2 m_applesPos[15];
    int m_effectId = 0;                 //音效play的id
    int m_InitAniIndex = 0;
    bool m_opentouched = false;
    bool m_jumped = false;
};


#endif /* defined(__superBaby__Round042_04__) */
