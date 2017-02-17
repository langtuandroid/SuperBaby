//
//  Round048_01.h
//  superBaby
//
//  Created by Administrator on 15/8/6.
//
//

#ifndef __superBaby__Round048_01__
#define __superBaby__Round048_01__

#include "RoundBase.h"
USING_NS_CC;

class Actor;
class StageScene;
class Round048_01 : public RoundBase
{
public:
    Round048_01();
    virtual ~Round048_01();
    
public:
    bool initWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    static Round048_01* createWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    
public:
    void cleanUp();
    virtual void onEnterRound();
    virtual void celebrate(const float);
    /* 开启触摸 */
    virtual void openTouch();
    /*  回合结束回调 */
    virtual void endRound();
    
    //依次下降出现答案
    void changeScaleAnswersSchedule(float);
        
private:
    Sprite* m_pAnswersSp[3];
    Vec2 m_answersPos[3];
    int m_InitAniIndex = 0;
    int m_effectId = 0;                  //音效play的id
    bool m_opentouched = false;
    bool m_jumped = false;
};


#endif /* defined(__superBaby__Round048_01__) */
