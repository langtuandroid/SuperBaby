//
//  Round034_04.h
//  superBaby
//
//  Created by Administrator on 15/6/23.
//
//

#ifndef __superBaby__Round034_04__
#define __superBaby__Round034_04__

#include "RoundBase.h"
#include <ui/CocosGUI.h>
USING_NS_CC;

class Actor;
class Round034_04 : public RoundBase
{
public:
    Round034_04();
    virtual ~Round034_04();
    
public:
    bool initWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    static Round034_04* createWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    
public:
    void cleanUp();
    virtual void onEnterRound();
    virtual void celebrate(const float);
    /* 开启触摸 */
    virtual void openTouch();
    /*  回合结束回调 */
    virtual void endRound();
    
    //放大答案
    void changeScaleMaxAnswers();
    //缩小答案
    void changeScaleSmallAnswers(float);

    //飞礼物
    void flyGift();
    
private:
    ui::ImageView* m_pAnswersImageView[9];  //答案图片
    Vec2 m_answersPos[9];               //答案坐标
    Sprite* m_pBoardSp = nullptr;       //物品架
    Sprite* m_pShadowSp = nullptr;
    Actor* m_pAnswerActor = nullptr;
    int m_effectId = 0;                //音效play的id
    GLubyte m_opacity = 0;
    bool m_opentouched = false;
    bool m_jumped = false;
    Actor* m_pPaopaoActor = nullptr;
    Actor* m_pWhiteLightActor = nullptr;
};


#endif /* defined(__superBaby__Round034_04__) */
