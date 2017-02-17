//
//  Round022_01.h
//  AbelLLExplorer
//
//  Created by Administrator on 15/5/22.
//
//

#ifndef __AbelLLExplorer__Round022_01__
#define __AbelLLExplorer__Round022_01__

#include "RoundBase.h"
#include "ui/CocosGUI.h"
USING_NS_CC;
using namespace ui;

class Actor;
class StageScene;
class Round022_01 : public RoundBase
{
public:
    Round022_01();
    virtual ~Round022_01();
    
public:
    bool initWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    static Round022_01* createWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    
public:
    
    
public:
    virtual void onEnterRound();
    virtual void celebrate(const float);
    /* 开启触摸 */
    virtual void openTouch();
    /*  回合结束回调 */
    virtual void endRound();
    
    //改变答案大小
    void changeScaleAnswersSchedule(float);
    
    //渐现提示出现
    void fadeHintsSchedule(float);
    
    //上升颜色板
    void upColorBoard();
    
    //答对下降颜色板
    void downColorBoard(float);
    
private:
    ImageView* m_pFlowerImageView;

    Sprite* m_pOverSp = nullptr;
    
    Sprite* m_pHintsSp[2];
    Vec2 m_hintsPos[2];
    Sprite* m_pColorBoard = nullptr;
    Sprite* m_pAnswersSp[2];
    Vec2 m_answersPos[2];
    Vec2 m_answersEndPos[2];
    Sprite* m_pDoorFrameSp = nullptr;
    Sprite* m_pUpWallSp = nullptr;
    Sprite* m_pLeftWallSp = nullptr;
    
    unsigned int m_InitAniIndex = 0;
    int m_effectId = 0;
    Vec2 m_originalPoint;
    Vec2 m_touchBeganPoint;         // 拖放开始位置
    unsigned int m_curCorrectNum = 0;
    
    int m_fristCorrectNum = -1;
    bool m_opentouched = false;
    bool m_jumped = false;
};

#endif /* defined(__AbelLLExplorer__Round022_01__) */
