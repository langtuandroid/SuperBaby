//
//  Round041_02.h
//  superBaby
//
//  Created by Administrator on 15/7/28.
//
//

#ifndef __superBaby__Round041_02__
#define __superBaby__Round041_02__

#include "RoundBase.h"
USING_NS_CC;

class Actor;
class StageScene;
class Round041_02 : public RoundBase
{
public:
    Round041_02();
    virtual ~Round041_02();
    
public:
    bool initWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    static Round041_02* createWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    
public:
    void cleanUp();
    virtual void onEnterRound();
    virtual void celebrate(const float);
    /* 开启触摸 */
    virtual void openTouch();
    /*  回合结束回调 */
    virtual void endRound();
    
    //依次放大答案以显示
    void changeScaleAnswersSchedule(float);
    
private:
    Sprite* m_pAnswersSp[4];
    Vec2 m_answersPos[4];
    int m_InitAniIndex = 0;
    Vec2 m_originalPoint;
    Vec2 m_touchBeganPoint;         // 拖放开始位置
    int m_effectId = 0;                //音效play的id
    bool m_opentouched = false;
    bool m_jumped = false;
};

#endif /* defined(__superBaby__Round041_02__) */