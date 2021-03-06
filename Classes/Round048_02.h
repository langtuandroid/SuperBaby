//
//  Round048_02.h
//  superBaby
//
//  Created by Administrator on 15/8/6.
//
//

#ifndef __superBaby__Round048_02__
#define __superBaby__Round048_02__

#include "RoundBase.h"
#include <ui/CocosGUI.h>
USING_NS_CC;

class Actor;
class StageScene;
class Round048_02 : public RoundBase
{
public:
    Round048_02();
    virtual ~Round048_02();
    
public:
    bool initWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    static Round048_02* createWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    
public:
    void cleanUp();
    virtual void onEnterRound();
    virtual void celebrate(const float);
    /* 开启触摸 */
    virtual void openTouch();
    /*  回合结束回调 */
    virtual void endRound();
    
    //放大显示答案
    void changeScaleAnswersSchedule(float);
    
    void changeScaleTopicsSchedule(float);
    
    void fadeOutAnswers(float);

private:
    Sprite* m_pAnswersSp[3];
    Vec2 m_answersPos[3];
    Vec2 m_answersEndPos[3];
    ui::ImageView* m_pTopicsImageView[3];
    Vec2 m_topicsPos[3];
    int m_effectId = 0;                //音效play的id
    int m_InitAniIndex = 0;
    bool m_opentouched = false;
    bool m_jumped = false;
    unsigned int m_curCorrectNum = 0;
    Vec2 m_originalPoint;
    Vec2 m_touchBeganPoint;         // 拖放开始位置
};


#endif /* defined(__superBaby__Round048_02__) */
