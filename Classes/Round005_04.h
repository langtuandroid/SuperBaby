//
//  Round005_04.h
//  AbelLLExplorer
//
//  Created by Administrator on 15/4/1.
//
//

#ifndef __AbelLLExplorer__Round005_04__
#define __AbelLLExplorer__Round005_04__

#include "RoundBase.h"

USING_NS_CC;

class Actor;
class StageScene;
class Round005_04 : public RoundBase
{
public:
    Round005_04();
    virtual ~Round005_04();
    
public:
    bool initWithRoundIndex(const int,StageScene*, const Vec2& pos = Vec2::ZERO);
    static Round005_04* createWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    
public:
    virtual void celebrate(const float);
    virtual void onPreLoadedRes();
    /* 开启触摸 */
    virtual void openTouch();
    /*  回合结束回调 */
    virtual void endRound();
    /*  处理npc收集 */
    virtual void processNpcCollect();
    
    //电话渐变出现
    void fadeInTelephones();
    //电话渐变出现完成
    void fadeInTelephonesComplete();
    //加载消防员
    void addFiremanActor();
    
    //电话顺序淡出
    void fadeOutTelephone();
    
private:
    void cleanUp();
    virtual void onEnterRound();
    
private:
    Sprite* m_pTelephones[3];                 //电话组
    
    Actor* m_pTentStandbyActor;                      //帐篷待机actor
    Actor* m_pTentTriggerActor;                      //帐篷触发actor
    Actor* m_PFiremanActor;                   //消防员actor
    
    int m_effectId;
    bool m_opentouched = false;
    bool m_jumped = false;
};


#endif /* defined(__AbelLLExplorer__Round005_04__) */
