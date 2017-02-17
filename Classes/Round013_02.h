//
//  Round013_02.h
//  AbelLLExplorer
//
//  Created by adinnet on 2015/3/9.
//
//  第十三关卡第二回合
//

#ifndef __AbelLLExplorer__Round013_02__
#define __AbelLLExplorer__Round013_02__

#include "RoundBase.h"

USING_NS_CC;

class Actor;
class NpcActor;
class StageScene;
class Round013_02 : public RoundBase
{
public:
    Round013_02();
    virtual ~Round013_02();
    
public:
    bool initWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    static Round013_02* createWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    
public:
    virtual void onEnterRound();
    virtual void celebrate(const float);
    virtual void onPreLoadedRes();
    /* 开启触摸 */
    virtual void openTouch();
    /*  回合结束回调 */
    virtual void endRound();
    /* 处理 npc 收集 */
    virtual void processNpcCollect();
    
    /* 播放过关引导 */
    void playPassActor();

private:
    /* 缩放完成回调 */
    void scaleCompleteCB();
    /* 登上火箭 */
    void goOnRocket();
    /*火箭起飞*/
    void rocketFly();
    
private:
    SpriteBatchNode* m_pSBN = nullptr;
    Sprite* m_numberSp[3];                  // 数字
    Sprite* m_LineSp[3];                    // 线
    Sprite* m_LineSpHL[3];                  // 线(高亮)
    Vec2 m_linePos[3];
    
    unsigned int m_numberIndex = 0;
    
    Actor* m_rocketActor = nullptr;         // 火箭 boss
    NpcActor* m_pNpcActor = nullptr;           // npc
    
    int m_effectId;                //音效play的id
};
#endif /* defined(__AbelLLExplorer__Round013_02__) */
