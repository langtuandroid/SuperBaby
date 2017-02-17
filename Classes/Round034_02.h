//
//  Round034_02.h
//  superBaby
//
//  Created by Administrator on 15/6/23.
//
//

#ifndef __superBaby__Round034_02__
#define __superBaby__Round034_02__

#include "RoundBase.h"
#include <ui/CocosGUI.h>

class Actor;
class StageScene;
class Round034_02 : public RoundBase
{
public:
    Round034_02();
    virtual ~Round034_02();
    
public:
    bool initWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    static Round034_02* createWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    
public:
    void cleanUp();
    virtual void onEnterRound();
    virtual void celebrate(const float);
    /* 开启触摸 */
    virtual void openTouch();
    /*  回合结束回调 */
    virtual void endRound();
    
    //依次放大答案以提示
    void changeScaleAnswersSchedule(float);
    
    //依次放大题目以提示
    void changeScaleTopicSchedule(float);
    
    //答对后移动船
    void moveShip();
    
private:
    cocos2d::Sprite* m_pBoardSp = nullptr;
    cocos2d::ui::ImageView * m_pAnswersImageView[3];
    std::string m_answersNormalStr[3];
    std::string m_answersStr[3];
    Vec2 m_answersPos[3];
    cocos2d::ui::ImageView* m_pTopicsImageView[3];
    std::string m_topicsNormalStr[3];
    std::string m_topicsStr[3];
    Vec2 m_topicsPos[3];
    Sprite* m_pShipSp = nullptr;
    Sprite* m_pMountSp = nullptr;
    int m_InitAniIndex = 0;
    int m_InitIndex = 0;
    int m_effectId = 0;                //音效play的id
    bool m_opentouched = false;
    bool m_jumped = false;
    Actor* m_pPaopaoActor = nullptr;
    Actor* m_pWhiteLightActor = nullptr;
};


#endif /* defined(__superBaby__Round034_02__) */
