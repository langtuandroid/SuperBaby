//
//  Round014_02.h
//  AbelLLExplorer
//
//  Created by Administrator on 15/5/12.
//
//

#ifndef __AbelLLExplorer__Round014_02__
#define __AbelLLExplorer__Round014_02__

#include "RoundBase.h"

USING_NS_CC;

class Actor;
class StageScene;
class Round014_02 : public RoundBase
{
public:
    Round014_02();
    virtual ~Round014_02();
    
public:
    bool initWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    static Round014_02* createWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    
public:
    void cleanUp();
    virtual void onEnterRound();
    virtual void celebrate(const float);
    /* 开启触摸 */
    virtual void openTouch();
    /*  回合结束回调 */
    virtual void endRound();
    
    //依次改变星球大小
    void scaleChangeSchedule(float);
    
    //上移机器人
    void upMoveRobot();
    
    //渐变显隐目标
    void fadeTarget(float);
    
private:
    SpriteBatchNode* m_pBN = nullptr;
    
    Sprite* m_pStarBalls[4];            //星球
    
    Vec2 m_starBallsPos[4];
    Vec2 m_starBallsEndPos[4];            //星球随机后最终坐标
    Vec2 m_targetsPos[2];               //目的坐标

    
    Sprite* m_pRobot;                   //机器人
    
    Sprite* m_ptargetHintSp[2];            //目标提示sp
        
    Sprite* m_pBeforeSp;                //前景
    Sprite* m_pWallSp;                  //墙
    Sprite* m_pRoadSp;                  //路

    int m_InitAniIndex;
    Vec2 m_originalPoint;
    Vec2 m_touchBeganPoint;         // 拖放开始位置
    int m_effectId = 0;                //音效play的id
    
    unsigned int m_curSameBallRandNum = 0;       //当前相同球的随机编号
    unsigned int m_curCorrectNumber = 0;
    int m_finishedNumber = -1;                       //已经完成的补充
    bool m_opentouched = false;
    bool m_jumped = false;
};

#endif /* defined(__AbelLLExplorer__Round014_02__) */
