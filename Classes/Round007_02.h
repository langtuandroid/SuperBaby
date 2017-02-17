//
//  Round007_02.h
//  AbelLLExplorer
//
//  Created by Administrator on 15/4/3.
//
//

#ifndef __AbelLLExplorer__Round007_02__
#define __AbelLLExplorer__Round007_02__

#include "RoundBase.h"
USING_NS_CC;

class Actor;
class StageScene;
class Round007_02 : public RoundBase
{
public:
    Round007_02();
    virtual ~Round007_02();
    
public:
    bool initWithRoundIndex(const int,StageScene*, const Vec2& pos = Vec2::ZERO);
    static Round007_02* createWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    
public:
    virtual void celebrate(const float);
    virtual void onPreLoadedRes();
    virtual void openTouch();
    /*  回合结束回调 */
    virtual void endRound();
    // 回合开场动画，图形上升动画调度
    void graphsUpSchedule(float);
    //回合开场渐变出现蓝色目标图形
    void fadeInBlueGraphs();
    //全部答对后改变狐狸动作
    void changeFoxActor();
    
private:
    void cleanUp();
    virtual void onEnterRound();
    
private:
    SpriteBatchNode* m_pBN;     //BN
    Sprite* m_pBlueGraphs[3];  //蓝色图形组
    Sprite* m_pYellowGraphs[3]; //黄色图形组
    Sprite* m_pYellowFinshGraphs[3]; //完成答题状态黄色图形组
    Actor* m_pFoxActor;         //狐狸actor
    Actor* m_pWaterfallActor = nullptr;
    
    Vec2 m_yellowGraphPos[3];   //黄色图形组坐标组
    Vec2 m_blueGraphPos[3];     //蓝色图形坐标组
    Vec2 m_yellowFinshGraphPos[3];  //完成答题状态黄色图形坐标组

    Vec2 m_originalPoint;
    Vec2 m_touchBeganPoint;
    int m_correctNumber;        //当前答对个数
    unsigned int m_InitAniIndex;
    int m_effectId;
    Vec2 m_originalPoints[3];
    bool m_opentouched = false;
    bool m_jumped = false;
};

#endif /* defined(__AbelLLExplorer__Round007_02__) */
