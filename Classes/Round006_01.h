//
//  Round006_01.h
//  AbelLLExplorer
//
//  Created by Administrator on 15/4/2.
//
//

#ifndef __AbelLLExplorer__Round006_01__
#define __AbelLLExplorer__Round006_01__

#include "RoundBase.h"
USING_NS_CC;

class Actor;
class StageScene;
class Round006_01 : public RoundBase
{
public:
    Round006_01();
    virtual ~Round006_01();
    
public:
    bool initWithRoundIndex(const int,StageScene*, const Vec2& pos = Vec2::ZERO);
    static Round006_01* createWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    
public:
    virtual void celebrate(const float);
    virtual void onPreLoadedRes();
    /* 开启触摸 */
    virtual void openTouch();
    /*  回合结束回调 */
    virtual void endRound();
    //盒子渐变出现
    void fadeInBoxs();
    //盒子渐变消失
    void fadeOutBoxs();
    
    // 回合开场动画，物品下坠动画调度
    void goodsDonwSchedule(float);
    
    //背景云渐变消失
    void fadeOutCloud();
    
private:
    virtual void onEnterRound();
    
private:
    SpriteBatchNode* m_pGoodsBN = nullptr;     //物品BN
    Sprite* m_pGoods[3];             //物品组
    Sprite* m_pEndGoods[3];            //最终的物品组
    Sprite* m_pLeftBoxsBack = nullptr;          //左边箱子后面
    Sprite* m_pLeftBoxs = nullptr;              //左边箱子前面
    Sprite* m_pRightBoxsBack = nullptr;         //右边箱子后面
    Sprite* m_pRightBoxs = nullptr;             //右边箱子前面
    Actor* m_pCloudActor = nullptr;            //背景云actor
    Vec2 m_goodsPos[3];              //物品位置组
    int m_curGoodsIndex = 0;
    Vec2 m_originalPoint;
    Vec2 m_touchBeganPoint;
    int m_correctNumber = 0;
    int m_effectId = 0;
    Vec2 m_originalPoints[3];
    bool m_opentouched = false;
    bool m_jumped = false;
};

#endif /* defined(__AbelLLExplorer__Round006_01__) */
