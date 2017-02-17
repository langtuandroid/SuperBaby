//
//  Round004_02.h
//  AbelLLExplorer
//
//  Created by Administrator on 2015/3/27.
//
//

#ifndef __AbelLLExplorer__Round004_02__
#define __AbelLLExplorer__Round004_02__

#include "RoundBase.h"

USING_NS_CC;

class Actor;
class StageScene;
class Round004_02 : public RoundBase
{
public:
    Round004_02();
    virtual ~Round004_02();
    
public:
    bool initWithRoundIndex(const int,StageScene*, const Vec2& pos = Vec2::ZERO);
    static Round004_02* createWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    
public:
    virtual void celebrate(const float);
    /* 开启触摸 */
    virtual void openTouch();
    /*  回合结束回调 */
    virtual void endRound();
    /*  处理npc收集 */
    virtual void processNpcCollect();
    
private:
    void cleanUp();
    virtual void onEnterRound();
private:
    Sprite* m_pTents[5];               //帐篷组
    Sprite* m_pTempTents[5];            //
    Vec2 m_pTentsPos[5];                  //帐篷组位置
    EventListenerTouchOneByOne* listenerRight;
    
    Image* m_pImages[5];               //帐篷小图组
    Image* m_pTempImages[5];               //帐篷小图组
    int m_curSelIndex;                 //选中帐篷索引
    
    int m_effectId;
};

#endif /* defined(__AbelLLExplorer__Round004_02__) */
