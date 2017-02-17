//
//  Round045_03.h
//  superBaby
//
//  Created by Administrator on 15/8/3.
//
//

#ifndef __superBaby__Round045_03__
#define __superBaby__Round045_03__

#include "RoundBase.h"
#include <ui/CocosGUI.h>
USING_NS_CC;

class Actor;
class StageScene;
class Round045_03 : public RoundBase
{
public:
    Round045_03();
    virtual ~Round045_03();
    
public:
    bool initWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    static Round045_03* createWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    
public:
    void cleanUp();
    virtual void onEnterRound();
    virtual void celebrate(const float);
    /* 开启触摸 */
    virtual void openTouch();
    /*  回合结束回调 */
    virtual void endRound();
    //改变答案大小
    void changeScaleAnswer();
    
    void changeScaleHint();
    
    void changeActor();
    
private:
    Sprite* m_pAnswerSp = nullptr;
    ui::ImageView* m_pHintImageView = nullptr;
    Sprite* m_pShawdowSp = nullptr;
    Actor* m_pMSActor= nullptr;
    int m_effectId = 0;                //音效play的id
    Vec2 m_originalPoint;
    Vec2 m_touchBeganPoint;         // 拖放开始位置
    bool m_opentouched = false;
    bool m_jumped = false;
};

#endif /* defined(__superBaby__Round045_03__) */
