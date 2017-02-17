//
//  Round001_03.h
//  AbelLLExplorer
//
//  Created by adinnet on 2015/3/9.
//
//  第一个关卡第三回合
//

#ifndef __AbelLLExplorer__Round001_03__
#define __AbelLLExplorer__Round001_03__

#include "RoundBase.h"

USING_NS_CC;

class Actor;
class StageScene;
class Round001_03 : public RoundBase
{
public:
    Round001_03();
    virtual ~Round001_03();
    
public:
    bool initWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    static Round001_03* createWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    
public:
    virtual void onEnterRound();
    virtual void celebrate(const float);
    virtual void onPreLoadedRes();
    /* 开启触摸 */
    virtual void openTouch();
    /*  回合结束回调 */
    virtual void endRound();
    
    //庆祝完成后隐藏物品
    void fadeOutGoodsCelebrateComplete();
    
    //庆祝完成后改变兔子
    void fadeOutRabbitCelebrateComplete();
    
    //改变物品大小
    void changeScaleGoodsSchedule(float);
    
private:
    Rect m_answerRect;                      // 目标答案矩形
    
    Vec2 m_originalPoint;
    Vec2 m_touchBeganPoint;
        
    SpriteBatchNode* m_pGoodsBN = nullptr;
    Vec2 m_goodsInitPos[4];                 //物品初始位置
    
    Sprite* m_goodsSp[4];                   // 物品
    Sprite* m_targetGoodsSp[3];             // 目标物品
    
    Actor* m_rabbit = nullptr;              // 兔子 boss
    Actor* m_pNpcActor = nullptr;           // npc
    
    // 拖放答题成功的数量
    unsigned int m_numberOfdragComplete = 0;
    
    int m_effectId = 0;                //音效play的id
    int m_InitAniIndex = 0;
    bool m_opentouched = false;
    bool m_jumped = false;
};
#endif /* defined(__AbelLLExplorer__Round001_03__) */
