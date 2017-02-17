//
//  Round008_03.h
//  AbelLLExplorer
//
//  Created by Administrator on 2015/4/7.
//
//

#ifndef __AbelLLExplorer__Round008_03__
#define __AbelLLExplorer__Round008_03__

#include "RoundBase.h"
USING_NS_CC;

class Actor;
class StageScene;
class Round008_03 : public RoundBase
{
public:
    Round008_03();
    virtual ~Round008_03();
    
public:
    bool initWithRoundIndex(const int,StageScene*, const Vec2& pos = Vec2::ZERO);
    static Round008_03* createWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    
public:
    virtual void celebrate(const float);
    virtual void onEnterRound();
    virtual void onPreLoadedRes();
    virtual void openTouch();
    /*  回合结束回调 */
    virtual void endRound();

public:
    //回合开始，黄色框左边进入
    void downMoveSchedule(float);
    //回合开始，箭头上面掉下
    void arrowsDownSchedule(float);
    //回合开始，青蛙气泡升起
    void frogUpSchedule(float);
    //黄色框和箭头一次放大缩小
    void yellowFramesChangeScaleSchedule(float);
    //青蛙气泡上下浮动
    void moveFrogBubbleSchedule(float);
    
private:
    
private:
    SpriteBatchNode* m_pBN;     //BN
    Sprite* m_pFrogs[4];        //青蛙的各个阶段
    Sprite* m_pYellowFrames[4]; //黄色目标框组
    Sprite* m_pArrows[3];       //上层箭头组
    Sprite* m_pDownArrows[3];     //下移箭头组
    Sprite* m_pFinishFrogs[4];    //完成的青蛙阶段图
    
    Vec2 m_frogsPos[4];           //青蛙各个阶段sp的坐标组
    Vec2 m_yellowFramesPos[4];    //黄色目标框坐标组
    Vec2 m_finishYellowFramesPos[4];    //完成状态黄色目标框坐标组
    Vec2 m_arrowsPos[3];          //箭头坐标组（上层）
    Vec2 m_downArrowsPos[3];      //下降箭头坐标组（下层）

    Vec2 m_originalPoint;
    Vec2 m_touchBeganPoint;
    int m_correctNumber;
    unsigned int m_InitAniIndex;
    unsigned int m_yellowFrameIndex;
    unsigned int m_arrowIndex;
    int m_effectId;
    Vec2 m_originalPoints[4];
    bool m_opentouched = false;
    bool m_jumped = false;
};

#endif /* defined(__AbelLLExplorer__Round008_03__) */
