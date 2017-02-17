//
//  Round016_02.h
//  AbelLLExplorer
//
//  Created by Administrator on 15/5/15.
//
//

#ifndef __AbelLLExplorer__Round016_02__
#define __AbelLLExplorer__Round016_02__

#include "RoundBase.h"
#include <ui/CocosGUI.h>
USING_NS_CC;
using namespace ui;
class Actor;
class StageScene;
class Round016_02 : public RoundBase
{
public:
    Round016_02();
    virtual ~Round016_02();
    
public:
    bool initWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    static Round016_02* createWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    
public:
    void cleanUp();
    virtual void onEnterRound();
    virtual void celebrate(const float);
    /* 开启触摸 */
    virtual void openTouch();
    /*  回合结束回调 */
    virtual void endRound();
    
    //渐变提示框
    void fadeHintSchedule(float);
    
    //答对后上升板
    void upBoard();
    
    //答对后渐隐剩余蝴蝶
    void fadeOutButterfly();
    
private:
    SpriteBatchNode* m_pBN = nullptr;
    
    ImageView* m_pButterflys[3];            //蝴蝶
    Vec2 m_butterflysPos[3];
    Vec2 m_butterflyEndPos[3];
    
    Sprite* m_pBoardSp;                 //题目板
    
    Sprite* m_pHintSp;                //目标提示sp
    
    int m_InitAniIndex;
    int m_effectId = 0;                //音效play的id
    int m_curCorrectNumber = 0;                //当前答对个数
    Vec2 m_originalPoint;
    Vec2 m_touchBeganPoint;         // 拖放开始位置
};

#endif /* defined(__AbelLLExplorer__Round016_02__) */
