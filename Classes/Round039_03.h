//
//  Round039_03.h
//  superBaby
//
//  Created by Administrator on 15/7/23.
//
//

#ifndef __superBaby__Round039_03__
#define __superBaby__Round039_03__

#include "RoundBase.h"
#include <ui/CocosGUI.h>
USING_NS_CC;

class Actor;
class StageScene;
class Round039_03 : public RoundBase
{
public:
    Round039_03();
    virtual ~Round039_03();
    
public:
    bool initWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    static Round039_03* createWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    
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
    
    void changeScaleMarksSchedule(float);
    
    void fadeOutAnswers();
    
private:
    Sprite* m_pAnswersSp[3];
    Vec2 m_answersPos[3];
    Vec2 m_answersEndPos[3];
    ui::ImageView* m_pBottlesImageView;
    Sprite* m_pMarksSp[3];
    Vec2 m_marksPos[3];
    int m_effectId = 0;                 //音效play的id
    int m_curSelIndex = -1;              //
    int m_InitAniIndex = 0;
    bool m_opentouched = false;
    bool m_jumped = false;
    unsigned int m_randNum = 0;
    unsigned int m_curCorrectNum = 0;
    Vec2 m_originalPoint;
    Vec2 m_touchBeganPoint;         // 拖放开始位置
};

#endif /* defined(__superBaby__Round039_03__) */
