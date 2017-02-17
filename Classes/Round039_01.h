//
//  Round039_01.h
//  superBaby
//
//  Created by Administrator on 15/7/23.
//
//

#ifndef __superBaby__Round039_01__
#define __superBaby__Round039_01__

#include "RoundBase.h"
#include <ui/CocosGUI.h>
USING_NS_CC;

class Actor;
class StageScene;
class Round039_01 : public RoundBase
{
public:
    Round039_01();
    virtual ~Round039_01();
    
public:
    bool initWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    static Round039_01* createWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    
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
    
    void changeMarkSchedule(float);
    
    void fadeAnswers();
    
    void changeCar();
    
private:
    Sprite* m_pAnswersSp[3];
    Vec2 m_answersPos[3];
    Vec2 m_answersEndPos[3];
    ui::ImageView* m_pMarkImageView = nullptr;
    ui::ImageView* m_pCarImageView = nullptr;
    int m_InitAniIndex = 0;
    Vec2 m_originalPoint;
    Vec2 m_touchBeganPoint;         // 拖放开始位置
    int m_effectId = 0;                //音效play的id
    bool m_opentouched = false;
    bool m_jumped = false;
};


#endif /* defined(__superBaby__Round039_01__) */
