//
//  Round010_03.h
//  AbelLLExplorer
//
//  Created by Administrator on 15/4/8.
//
//

#ifndef __AbelLLExplorer__Round010_03__
#define __AbelLLExplorer__Round010_03__

#include "RoundBase.h"
USING_NS_CC;

class Actor;
class StageScene;
class Round010_03 : public RoundBase
{
public:
    Round010_03();
    virtual ~Round010_03();
    
public:
    bool initWithRoundIndex(const int,StageScene*, const Vec2& pos = Vec2::ZERO);
    static Round010_03* createWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    
public:
    virtual void celebrate(const float);
    virtual void onEnterRound();
    virtual void onPreLoadedRes();
    virtual void openTouch();
    /*  回合结束回调 */
    virtual void endRound();

    //回合开始，动物图右移出现
    void animalMoveSchedule(float);
    //答对之后，动物图变小渐变消失
    void animalFadeOut(int);
    //回合结束，怪物actor改变动作
    void changeMonsterActor(float);

private:
    void cleanUp();
    
    
private:
    
    SpriteBatchNode* m_pBN;     //BN
    Sprite* m_pAnimals[4];      //动物组
    Actor* m_pMonsterActor;          //怪物actor
    Actor* m_pTouchEffectActor;      //点击效果actor
    
    Vec2 m_partsPos[3];       //怪物身体部位坐标组
    Vec2 m_originalPoint;
    Vec2 m_touchBeganPoint;
    unsigned int m_InitAniIndex;
    int m_correctNumber;
    int m_effectId;
    bool m_opentouched = false;
    bool m_jumped = false;
};

#endif /* defined(__AbelLLExplorer__Round010_03__) */
