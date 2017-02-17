//
//  Round008_02.h
//  AbelLLExplorer
//
//  Created by Administrator on 2015/4/7.
//
//

#ifndef __AbelLLExplorer__Round008_02__
#define __AbelLLExplorer__Round008_02__

#include "RoundBase.h"
USING_NS_CC;

class Actor;
class StageScene;
class Round008_02 : public RoundBase
{
public:
    Round008_02();
    virtual ~Round008_02();
    
public:
    bool initWithRoundIndex(const int,StageScene*, const Vec2& pos = Vec2::ZERO);
    static Round008_02* createWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    
public:
    virtual void celebrate(const float);
    virtual void onPreLoadedRes();
    virtual void openTouch();
    /*  回合结束回调 */
    virtual void endRound();
    //从右边飞出食物
    void leftMoveSchedule(float);
    
private:
    void cleanUp();
    virtual void onEnterRound();
    
private:
    SpriteBatchNode* m_pBN;     //BN
    Sprite* m_pFoods[3];        //食物组
    Actor*  m_pAnimalsActor[3];  //动物组
    
    Vec2 m_foodsPos[3];          //食物坐标组
    Vec2 m_animalsPos[3];        //动物坐标组
    
    Rect m_animalsRects[3];      //动物触摸响应区域
    Vec2 m_centerRectsPos[3];    //触摸区域中心点坐标组
    Vec2 m_originalPoint;
    Vec2 m_touchBeganPoint;
    int m_correctNumber;
    unsigned int m_InitAniIndex;
    int m_effectId;
    Vec2 m_originalPoints[3];
    bool m_opentouched = false;
    bool m_jumped = false;
};

#endif /* defined(__AbelLLExplorer__Round008_02__) */
