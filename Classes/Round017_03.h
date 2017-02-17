//
//  Round017_03.h
//  AbelLLExplorer
//
//  Created by Administrator on 15/5/18.
//
//

#ifndef __AbelLLExplorer__Round017_03__
#define __AbelLLExplorer__Round017_03__

#include "RoundBase.h"
#include "ui/CocosGUI.h"
USING_NS_CC;
using namespace ui;

class Actor;
class StageScene;
class Round017_03 : public RoundBase
{
public:
    Round017_03();
    virtual ~Round017_03();
    
public:
    bool initWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    static Round017_03* createWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    
public:
    void cleanUp();
    virtual void onEnterRound();
    virtual void celebrate(const float);
    /* 开启触摸 */
    virtual void openTouch();
    /*  回合结束回调 */
    virtual void endRound();
    
    //依次放大答案，以提示
    void changeScaleAnswersSchedule(float);
    
    //答对后隐藏剩余答案
    void fadeOutAnswers();
    
    //移动箭头
    void rightMoveArrows();
    
private:
    Sprite* m_pAnswerRopes[3];            //答案绳子
    Vec2 m_answerRopesPos[3];
    
    Sprite* m_pArrowsSp;                //箭头
    Sprite* m_pBoardSp;
    int m_InitAniIndex;
    int m_effectId = 0;                //音效play的id
    bool m_opentouched = false;
    bool m_jumped = false;
};


#endif /* defined(__AbelLLExplorer__Round017_03__) */
