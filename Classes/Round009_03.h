//
//  Round009_03.h
//  AbelLLExplorer
//
//  Created by Administrator on 2015/4/3.
//
//

#ifndef __AbelLLExplorer__Round009_03__
#define __AbelLLExplorer__Round009_03__

#include "RoundBase.h"
#include "ui/CocosGUI.h"
USING_NS_CC;
using namespace ui;

class Actor;
class StageScene;
class Round009_03 : public RoundBase
{
public:
    Round009_03();
    virtual ~Round009_03();
    
public:
    bool initWithRoundIndex(const int,StageScene*, const Vec2& pos = Vec2::ZERO);
    static Round009_03* createWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    
public:
    virtual void celebrate(const float);
    virtual void onEnterRound();
    virtual void openTouch();
    /*  回合结束回调 */
    virtual void endRound();
    void changeScaleSchedule(float);

private:
    void addListener();
    
private:
    SpriteBatchNode* m_pSBN;
    ImageView* m_pDraw;
    Vec2 m_originalPoint;
    Vec2 m_touchBeganPoint;
    unsigned int m_correctNumber;
    
    // 碎片
    Sprite* m_chipSp[3];
    // 坐标系是画
    Rect m_targetRect;
    int m_effectId;
    Vec2 m_originalPoints[3];
    int m_InitAniIndex = 0;
    bool m_opentouched = false;
    bool m_jumped = false;
};

#endif /* defined(__AbelLLExplorer__Round009_03__) */
