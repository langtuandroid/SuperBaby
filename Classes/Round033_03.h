//
//  Round033_03.h
//  AbelLLExplorer
//
//  Created by Administrator on 15/6/10.
//
//

#ifndef __AbelLLExplorer__Round033_03__
#define __AbelLLExplorer__Round033_03__

#include "RoundBase.h"

USING_NS_CC;

class Actor;
class StageScene;
class Round033_03 : public RoundBase
{
public:
    Round033_03();
    virtual ~Round033_03();
    
public:
    bool initWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    static Round033_03* createWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    
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
    
    void changeColorSchedule(float);
    
    void downAnswersSchedule(float);
    
private:
    Sprite* m_pAnswersSp[3];
    Vec2 m_answersPos[3];
    Sprite* m_pBoardSp = nullptr;
    Sprite* m_pUpKitsSp[3];
    Vec2 m_upKitsPos[3];
    Sprite* m_pDownKitsSp[4];
    Vec2 m_downKitsPos[4];
    int m_effectId = 0;                 //音效play的id
    int m_curSelIndex = -1;              //
    int m_InitAniIndex = 0;
    bool m_opentouched = false;
    bool m_jumped = false;
    Actor* m_pPaopaoActor = nullptr;
    Actor* m_pWhiteLightActor = nullptr;
};


#endif /* defined(__AbelLLExplorer__Round033_03__) */
