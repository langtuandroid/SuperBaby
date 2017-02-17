//
//  Round045_02.h
//  superBaby
//
//  Created by Administrator on 15/8/3.
//
//

#ifndef __superBaby__Round045_02__
#define __superBaby__Round045_02__

#include "RoundBase.h"
#include <ui/CocosGUI.h>
USING_NS_CC;

class Actor;
class StageScene;
class Round045_02 : public RoundBase
{
public:
    Round045_02();
    virtual ~Round045_02();
    
public:
    bool initWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    static Round045_02* createWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    
public:
    void cleanUp();
    virtual void onEnterRound();
    virtual void celebrate(const float);
    /* 开启触摸 */
    virtual void openTouch();
    /*  回合结束回调 */
    virtual void endRound();
    /*  处理npc收集 */
    virtual void processNpcCollect();
    
    //改变答案大小以显示
    void changeScaleAnswersSchedule(float);
    
    void changeScaleHint();
    
    void changeActor();
    
private:
    Sprite* m_pAnswersSp[3];
    Vec2 m_answersPos[3];
    Vec2 m_answersEndPos[3];
    Sprite* m_pShawdowSp = nullptr;
    Actor* m_pMSActor = nullptr;
    ui::ImageView* m_pHintImageView = nullptr;
    int m_InitAniIndex = 0;
    Vec2 m_originalPoint;
    Vec2 m_touchBeganPoint;         // 拖放开始位置
    int m_effectId = 0;                //音效play的id
    bool m_opentouched = false;
    bool m_jumped = false;
};


#endif /* defined(__superBaby__Round045_02__) */
