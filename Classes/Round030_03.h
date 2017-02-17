//
//  Round030_03.h
//  AbelLLExplorer
//
//  Created by Administrator on 15/6/8.
//
//

#ifndef __AbelLLExplorer__Round030_03__
#define __AbelLLExplorer__Round030_03__

#include "RoundBase.h"
#include <ui/CocosGUI.h>
USING_NS_CC;
using namespace ui;
class Actor;
class StageScene;
class Round030_03 : public RoundBase
{
public:
    Round030_03();
    virtual ~Round030_03();
    
public:
    bool initWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    static Round030_03* createWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    
public:
    void cleanUp();
    virtual void onEnterRound();
    virtual void celebrate(const float);
    /* 开启触摸 */
    virtual void openTouch();
    /*  回合结束回调 */
    virtual void endRound();
    //依次放大贝壳以提示
    void changeScaleshellsSchedule(float);
    
    //答对移动贝壳
    void moveShells();
    
private:
    ImageView* m_pShellsImageView[5];        //船
    Vec2 m_shellsPos[5];
    int m_effectId = 0;                //音效play的id
    int m_InitAniIndex = 0;
    bool m_opentouched = false;
    bool m_jumped = false;
    int m_curCorrectNum = 0;
    Actor* m_pPaopaoActor = nullptr;
    Actor* m_pWhiteLightActor = nullptr;
};


#endif /* defined(__AbelLLExplorer__Round030_03__) */
