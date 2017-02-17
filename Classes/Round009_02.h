//
//  Round009_02.h
//  AbelLLExplorer
//
//  Created by Administrator on 2015/4/3.
//
//

#ifndef __AbelLLExplorer__Round009_02__
#define __AbelLLExplorer__Round009_02__

#include "RoundBase.h"
USING_NS_CC;

class Actor;
class StageScene;
class Round009_02 : public RoundBase
{
public:
    Round009_02();
    virtual ~Round009_02();
    
public:
    bool initWithRoundIndex(const int,StageScene*, const Vec2& pos = Vec2::ZERO);
    static Round009_02* createWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    
public:
    virtual void onEnterRound();
    virtual void celebrate(const float);
    virtual void onPreLoadedRes();
    virtual void openTouch();
    /*  回合结束回调 */
    virtual void endRound();
    
    //突出盒子
    void scaleChangeBox();

private:
    void addListener();
    
private:
    SpriteBatchNode* m_pSBN;

    int m_correctNumber;
    
    Actor* m_pCiweiActor;
    Actor* m_pSongshuActor;
    
    Sprite* m_targetSp[3];
    
    Vec2 m_originalPoint;           // 原来的位置
    Vec2 m_touchBeganPoint;         // 拖放开始位置
    Rect m_targetRect;
    int m_effectId;
    Vec2 m_originalPoints[3];
    bool m_opentouched = false;
    bool m_jumped = false;
};

#endif /* defined(__AbelLLExplorer__Round009_02__) */
