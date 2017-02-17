//
//  Round010_02.h
//  AbelLLExplorer
//
//  Created by Administrator on 15/4/8.
//
//

#ifndef __AbelLLExplorer__Round010_02__
#define __AbelLLExplorer__Round010_02__

#include "RoundBase.h"
USING_NS_CC;

class Actor;
class StageScene;
class Round010_02 : public RoundBase
{
public:
    Round010_02();
    virtual ~Round010_02();
    
public:
    bool initWithRoundIndex(const int,StageScene*, const Vec2& pos = Vec2::ZERO);
    static Round010_02* createWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    
public:
    virtual void celebrate(const float);
    virtual void onEnterRound();
    virtual void onPreLoadedRes();
    virtual void openTouch();
    /*  回合结束回调 */
    virtual void endRound();

    //回合开始，依次上升数字
    void upNumberSchedule(float);
    //回合结束，依次下降数字
    void downNumberSchedule(float);
    //绳子出现时动作
    void ropeInAction();
    //离开回合，绳子消失
    void ropeOutAction();
    //答对时绳子状态改变
    void changeRope();
    
private:
    void cleanUp();    
    
private:
    
    SpriteBatchNode* m_pBN;     //BN
    Sprite* m_pUpBackground;  //上部背景图
    Sprite* m_pRope;          //绳子
    Sprite* m_pWhiteRope;     //白色描边绳子
    Sprite* m_pPassRope;      //过关状态的绳子
    Sprite* m_pNumbers[3];    //答案数字组

    Vec2 m_originalPoint;
    Vec2 m_touchBeganPoint;
    unsigned int m_InitAniIndex;
    int m_effectId;
    bool m_opentouched = false;
    bool m_jumped = false;
};

#endif /* defined(__AbelLLExplorer__Round010_02__) */
