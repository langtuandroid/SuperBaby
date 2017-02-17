//
//  Round015_02.h
//  AbelLLExplorer
//
//  Created by Administrator on 15/5/14.
//
//

#ifndef __AbelLLExplorer__Round015_02__
#define __AbelLLExplorer__Round015_02__

#include "RoundBase.h"

USING_NS_CC;

class Actor;
class StageScene;
class Round015_02 : public RoundBase
{
public:
    Round015_02();
    virtual ~Round015_02();
    
public:
    bool initWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    static Round015_02* createWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    
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
    
private:
    SpriteBatchNode* m_pBN = nullptr;
    
    Sprite* m_pGraphBoards[3];            //图形板
    
    Vec2 m_graphBoardsPos[3];
    
    Sprite* m_pHintSp[3];                //目标提示sp
    Vec2 m_hintsPos[3];
    
    int m_InitAniIndex;
    int m_effectId = 0;                //音效play的id
    int m_curCorrectNumber = 0;                //当前答对个数
    bool m_opentouched = false;
    bool m_jumped = false;
};

#endif /* defined(__AbelLLExplorer__Round015_02__) */
