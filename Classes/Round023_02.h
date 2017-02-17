//
//  Round023_02.h
//  AbelLLExplorer
//
//  Created by adinnet on 2015/3/9.
//
//  第 23 关卡第 2 回合
//

#ifndef __AbelLLExplorer__Round023_02__
#define __AbelLLExplorer__Round023_02__

#include "RoundBase.h"

USING_NS_CC;

class Actor;
class NpcActor;
class StageScene;
class Round023_02 : public RoundBase
{
public:
    Round023_02();
    virtual ~Round023_02();
    
public:
    bool initWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    static Round023_02* createWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    
public:
    virtual void onEnterRound();
    virtual void celebrate(const float);
    virtual void onPreLoadedRes();
    /* 开启触摸 */
    virtual void openTouch();
    /*  回合结束回调 */
    virtual void endRound();
    
private:
    SpriteBatchNode* m_pSBN = nullptr;
    Sprite* m_pCarrotsSps[6];               // 萝卜
    Sprite* m_pPigmentSp[2];                // 颜料
    Sprite* m_pNpcSp = nullptr;
    unsigned int m_numberIndex = 0;
    Actor* m_pBrushActor = nullptr;         // 刷子
    
    Vec2 m_originalPoints[2];
    Vec2 m_touchBeganPoint;                 // 拖放开始位置
    
    bool m_opentouched = false;
    bool m_scaled = false;
    
    int m_effectId;                //音效play的id
};
#endif /* defined(__AbelLLExplorer__Round023_02__) */
