//
//  Round005_01.h
//  AbelLLExplorer
//
//  Created by Administrator on 15/4/1.
//
//

#ifndef __AbelLLExplorer__Round005_01__
#define __AbelLLExplorer__Round005_01__

#include "RoundBase.h"
USING_NS_CC;

class Actor;
class StageScene;
class Round005_01 : public RoundBase
{
public:
    Round005_01();
    virtual ~Round005_01();
    
public:
    bool initWithRoundIndex(const int,StageScene*, const Vec2& pos = Vec2::ZERO);
    static Round005_01* createWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    
public:
    virtual void celebrate(const float);
    /* 开启触摸 */
    virtual void openTouch();
    /*  回合结束回调 */
    virtual void endRound();
private:
    void cleanUp();
    virtual void onEnterRound();

private:
    Actor* m_pFlowers[5];           //花的actor组
    Actor* m_pYellowLight[2];      //被选中黄光actor
    
    Rect m_flowerChooseRect[5];   //花点击区域组
    
    Vec2 m_yellowLightPos[2];     //黄色光gaf位置组
    
    int m_chooseTrueNum;          //当前已经选择正确的个数
    int m_effectId;
    
    unsigned int m_choosedNum = -1;
};

#endif /* defined(__AbelLLExplorer__Round005_01__) */
