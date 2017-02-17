//
//  Round015_04.h
//  AbelLLExplorer
//
//  Created by Administrator on 15/5/14.
//
//

#ifndef __AbelLLExplorer__Round015_04__
#define __AbelLLExplorer__Round015_04__

#include "RoundBase.h"

USING_NS_CC;

class Actor;
class StageScene;
class Round015_04 : public RoundBase
{
public:
    Round015_04();
    virtual ~Round015_04();
    
public:
    bool initWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    static Round015_04* createWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    
public:
    
    
public:
    virtual void onEnterRound();
    virtual void celebrate(const float);
    /* 开启触摸 */
    virtual void openTouch();
    /*  回合结束回调 */
    virtual void endRound();
    /*  处理npc收集 */
    virtual void processNpcCollect();
    //分享回调
    void shareCallback();
    
    //回合结束分享调用
    void roundEndShareCallBack(bool isShare);
    
    //答案出现时动作
    void answersAppearActionSchedule(float);
    
    //改变题目大小
    void scaleChangeSchedule(float);
    
    //答对后答案下降系列动作
    void chooseTrueAnswersActionSchedule(float);
    
private:
    SpriteBatchNode* m_pBN = nullptr;
    
    Sprite* m_pTopicsSp[3];
    Vec2 m_topicsPos[3];
    
    Sprite* m_pAnswersSp[3];
    Vec2 m_answersPos[3];
    
    int m_InitAniIndex = 0;
    int m_effectId = 0;
    
    Actor* m_pNpcActor = nullptr;
    bool m_opentouched = false;
    bool m_jumped = false;

};

#endif /* defined(__AbelLLExplorer__Round015_04__) */
