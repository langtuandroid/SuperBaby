//
//  Round046_01.h
//  superBaby
//
//  Created by Administrator on 15/8/4.
//
//

#ifndef __superBaby__Round046_01__
#define __superBaby__Round046_01__

#include "RoundBase.h"
#include <ui/CocosGUI.h>
USING_NS_CC;

class Actor;
class StageScene;
class Round046_01 : public RoundBase
{
public:
    Round046_01();
    virtual ~Round046_01();
    
public:
    bool initWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    static Round046_01* createWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    
public:
    void cleanUp();
    virtual void onEnterRound();
    virtual void celebrate(const float);
    /* 开启触摸 */
    virtual void openTouch();
    /*  回合结束回调 */
    virtual void endRound();
    
    //依次上升答案
    void upAnswersSchedule(float);
    
    void downAnswersSchedule(float);
    
    //上升气球
    void upTopics();
    
private:
    Sprite* m_pAnswersSp[3];
    Vec2 m_answersPos[3];
    ui::ImageView* m_pTopicsImageView[6];
    Vec2 m_topicsPos[6];
    int m_InitAniIndex = 0;
    int m_effectId = 0;                  //音效play的id
    bool m_opentouched = false;
    bool m_jumped = false;
    int m_curCorrectNum = 0;
};

#endif /* defined(__superBaby__Round046_01__) */
