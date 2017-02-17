//
//  Round043_03.h
//  superBaby
//
//  Created by Administrator on 15/7/31.
//
//

#ifndef __superBaby__Round043_03__
#define __superBaby__Round043_03__

#include "RoundBase.h"
#include <ui/CocosGUI.h>
USING_NS_CC;

class Actor;
class StageScene;
class Round043_03 : public RoundBase
{
public:
    Round043_03();
    virtual ~Round043_03();
    
public:
    bool initWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    static Round043_03* createWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    
public:
    void cleanUp();
    virtual void onEnterRound();
    virtual void celebrate(const float);
    /* 开启触摸 */
    virtual void openTouch();
    /*  回合结束回调 */
    virtual void endRound();
    
    //改变答案大小
    void changeScaleAnswersSchedule(float);
        
private:
    Sprite* m_pAnswersSp[4];
    Vec2 m_answersPos[4];
    int m_effectId = 0;                 //音效play的id
    int m_InitAniIndex = 0;
    bool m_opentouched = false;
    bool m_jumped = false;
};

#endif /* defined(__superBaby__Round043_03__) */
