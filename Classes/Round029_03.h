//
//  Round029_03.h
//  AbelLLExplorer
//
//  Created by Administrator on 15/6/4.
//
//

#ifndef __AbelLLExplorer__Round029_03__
#define __AbelLLExplorer__Round029_03__

#include "RoundBase.h"

USING_NS_CC;

class Actor;
class StageScene;
class Round029_03 : public RoundBase
{
public:
    Round029_03();
    virtual ~Round029_03();
    
public:
    bool initWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    static Round029_03* createWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    
public:
    void cleanUp();
    virtual void onEnterRound();
    virtual void celebrate(const float);
    /* 开启触摸 */
    virtual void openTouch();
    /*  回合结束回调 */
    virtual void endRound();
    
    void changeScaleShells();
    
    void changeScaleBacket(float);
    
    //答对后相关动作
    void chooseComplete();
    
private:
    Sprite* m_pShellsSp[4];            //答案图片
    Vec2 m_shellsPos[4];               //答案坐标
    Vec2 m_shellsEndPos[4];
    Sprite* m_pBasketSp = nullptr;
    int m_effectId = 0;                 //音效play的id
    int m_curSelIndex;                  //选中树索引
    int m_InitAniIndex = 0;
    Vec2 m_originalPoint;
    Vec2 m_touchBeganPoint;         // 拖放开始位置
    bool m_opentouched = false;
    bool m_jumped = false;
    Actor* m_pPaopaoActor = nullptr;
    Actor* m_pWhiteLightActor = nullptr;
};

#endif /* defined(__AbelLLExplorer__Round029_03__) */
