//
//  Round047_03.h
//  superBaby
//
//  Created by Administrator on 15/8/5.
//
//

#ifndef __superBaby__Round047_03__
#define __superBaby__Round047_03__

#include "RoundBase.h"
#include <ui/CocosGUI.h>
USING_NS_CC;

class Actor;
class StageScene;
class Round047_03 : public RoundBase
{
public:
    Round047_03();
    virtual ~Round047_03();
    
public:
    bool initWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    static Round047_03* createWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    
public:
    void cleanUp();
    virtual void onEnterRound();
    virtual void celebrate(const float);
    /* 开启触摸 */
    virtual void openTouch();
    /*  回合结束回调 */
    virtual void endRound();
    //改变答案大小出现
    void ChangeScaleAnswersSchedule(float);
    
    void changeScaleHint();
        
    void changeActor();
    
    void changeScaleAnswers();
    
private:
    Sprite* m_pAnswersSp[3];
    Vec2 m_answersPos[3];
    Vec2 m_anwersEndPos[3];
    Actor* m_pMSActor = nullptr;
    Sprite* m_pHintSp = nullptr;
    int m_effectId = 0;                //音效play的id
    Vec2 m_originalPoint;
    Vec2 m_touchBeganPoint;         // 拖放开始位置
    bool m_opentouched = false;
    bool m_jumped = false;
    int m_InitAniIndex = 0;
};


#endif /* defined(__superBaby__Round047_03__) */
