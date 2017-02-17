//
//  Round047_01.h
//  superBaby
//
//  Created by Administrator on 15/8/5.
//
//

#ifndef __superBaby__Round047_01__
#define __superBaby__Round047_01__

#include "RoundBase.h"
USING_NS_CC;

class Actor;
class StageScene;
class Round047_01 : public RoundBase
{
public:
    Round047_01();
    virtual ~Round047_01();
    
public:
    bool initWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    static Round047_01* createWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    
public:
    void cleanUp();
    virtual void onEnterRound();
    virtual void celebrate(const float);
    /* 开启触摸 */
    virtual void openTouch();
    /*  回合结束回调 */
    virtual void endRound();
    
    //依次下降出现答案
    void downAnswersSchedule(float);
    
    void showHintsSchedule(float);
    
    void changeScaleMark();
    
    void downAnswers();
    
private:
    Sprite* m_pAnswersSp[3];
    Vec2 m_answersPos[3];
    Vec2 m_answersEndPos[3];
    Sprite* m_pTopicsSp[7];
    Sprite* m_pBacksSp[7];
    Vec2 m_backsPos[7];
    Sprite* m_pHintsSp[2];
    Vec2 m_hintsPos[2];
    int m_InitAniIndex = 0;
    int m_effectId = 0;                  //音效play的id
    bool m_opentouched = false;
    bool m_jumped = false;
    int m_curCorrectNum = 0;
    unsigned int m_randNum[3];
    Vec2 m_originalPoint;
    Vec2 m_touchBeganPoint;         // 拖放开始位置
};


#endif /* defined(__superBaby__Round047_01__) */
