//
//  Round044_01.h
//  superBaby
//
//  Created by Administrator on 15/8/3.
//
//

#ifndef __superBaby__Round044_01__
#define __superBaby__Round044_01__

#include "RoundBase.h"
#include <ui/CocosGUI.h>
USING_NS_CC;

class Actor;
class StageScene;
class Round044_01 : public RoundBase
{
public:
    Round044_01();
    virtual ~Round044_01();
    
public:
    bool initWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    static Round044_01* createWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    
public:
    void cleanUp();
    virtual void onEnterRound();
    virtual void celebrate(const float);
    /* 开启触摸 */
    virtual void openTouch();
    /*  回合结束回调 */
    virtual void endRound();
    
    //依次放大答案
    void changeScaleAnswersSchedule(float);
    
private:
    Sprite* m_pAnswersSp[8];
    Vec2 m_answersPos[8];
    int m_InitAniIndex = 0;
    int m_effectId = 0;                  //音效play的id
    bool m_opentouched = false;
    bool m_jumped = false;
    int m_curCorrectNum = 0;
};


#endif /* defined(__superBaby__Round044_01__) */
