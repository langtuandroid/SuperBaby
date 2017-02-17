//
//  Round018_03.h
//  AbelLLExplorer
//
//  Created by Administrator on 15/5/18.
//
//

#ifndef __AbelLLExplorer__Round018_03__
#define __AbelLLExplorer__Round018_03__

#include "RoundBase.h"
#include "ui/CocosGUI.h"
USING_NS_CC;
using namespace ui;

class Actor;
class StageScene;
class Round018_03 : public RoundBase
{
public:
    Round018_03();
    virtual ~Round018_03();
    
public:
    bool initWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    static Round018_03* createWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    
public:
    void cleanUp();
    virtual void onEnterRound();
    virtual void celebrate(const float);
    /* 开启触摸 */
    virtual void openTouch();
    /*  回合结束回调 */
    virtual void endRound();
    
    //渐隐渐显以提示
    void fadeHint();
    
    //依次放大答案，以提示
    void changeScaleAnswersSchedule(float);
    
    //答对后隐藏剩余答案
    void fadeOutAnswers(float);
    
    //答对后改变actor动作
    void changeActor();
    
private:
    Sprite* m_pAnswersSp[3];            //答案
    Vec2 m_answersPos[3];
    Vec2 m_answersEndPos[3];
    Sprite* m_pHintSp;
    
    Sprite* m_pDrawbridgeSp;
    
    Actor* m_pArrowActor;                //箭头
    ImageView* m_pBoardImageView;
    int m_InitAniIndex;
    int m_effectId = 0;                //音效play的id
    Vec2 m_originalPoint;
    Vec2 m_touchBeganPoint;         // 拖放开始位置
    bool m_opentouched = false;
    bool m_jumped = false;
};


#endif /* defined(__AbelLLExplorer__Round018_03__) */
