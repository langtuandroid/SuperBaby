//
//  Round036_03.h
//  superBaby
//
//  Created by Administrator on 15/7/7.
//
//

#ifndef __superBaby__Round036_03__
#define __superBaby__Round036_03__

#include "RoundBase.h"

USING_NS_CC;

class Actor;
class LeadingActor;
class StageScene;
class Round036_03 : public RoundBase
{
public:
    Round036_03();
    virtual ~Round036_03();
    
public:
    bool initWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    static Round036_03* createWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    
public:
    void cleanUp();
    virtual void onEnterRound();
    virtual void celebrate(const float);
    /* 开启触摸 */
    virtual void openTouch();
    /*  回合结束回调 */
    virtual void endRound();
    //抖动垃圾
    void rotateRubbishesSchedule(float);
    
    //显隐标志
    void fadeMarksSchedule(float);
    
    //打开盖子
    void openCovers();
    
    //关闭盖子
    void closeCovers();
    
    //收集
    void collectRubbishes(Sprite*, const unsigned int);
    
private:
    Sprite* m_pRubbishesSp[2];
    Vec2 m_rubbishesPos[2];
    Vec2 m_rubbishesEndPos[2];
    Sprite* m_pTrashesSp[2];
    Vec2 m_trashesPos[2];
    Sprite* m_pCoversSp[2];
    Vec2 m_coversPos[2];
    Sprite* m_pMarksSp[2];
    Vec2 m_marksPos[2];
    Actor* m_pPaopaoActor = nullptr;
    Actor* m_pWhiteLightActor = nullptr;
    Vec2 m_originalPoint;
    Vec2 m_touchBeganPoint;         // 拖放开始位置
    int m_effectId = 0;                //音效play的id
    bool m_opentouched = false;
    bool m_jumped = false;
    int m_InitAniIndex = 0;
    int m_curCorrectNum = 0;

};


#endif /* defined(__superBaby__Round036_03__) */
