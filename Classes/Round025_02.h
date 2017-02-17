//
//  Round025_02.h
//  AbelLLExplorer
//
//  Created by Administrator on 15/5/6.
//
//

#ifndef __AbelLLExplorer__Round025_02__
#define __AbelLLExplorer__Round025_02__

#include "RoundBase.h"

USING_NS_CC;

class Actor;
class StageScene;
class Round025_02 : public RoundBase
{
public:
    Round025_02();
    virtual ~Round025_02();
    
public:
    bool initWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    static Round025_02* createWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    
public:
    void cleanUp();
    virtual void onEnterRound();
    virtual void celebrate(const float);
    /* 开启触摸 */
    virtual void openTouch();
    /*  回合结束回调 */
    virtual void endRound();
    
    //第一次左移海鸥
    void leftMoveSeaBirdFirst();
    
    //第二次左移海鸥
    void leftMoveSeaBirdSecond();
    
    //上升答案
    void upAnswerNumber(float);
    
    //下降答案
    void downAnswerNumber(float);
    
private:
    SpriteBatchNode* m_pBN = nullptr;

    Actor* m_pSeaBirdsActor[6];        //海鸥
    
    Sprite* m_pAnswersNumber[3];       //答案
    
    Vec2 m_seaBirdsPos[6];
    
    Vec2 m_answersNumberPos[3];         
    
    int m_effectId = 0;                //音效play的id
    int m_InitAniIndex;
    Actor* m_pBoguangActor = nullptr;

};

#endif /* defined(__AbelLLExplorer__Round025_02__) */
