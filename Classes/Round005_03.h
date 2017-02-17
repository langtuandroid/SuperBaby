//
//  Round005_03.h
//  AbelLLExplorer
//
//  Created by Administrator on 15/4/1.
//
//

#ifndef __AbelLLExplorer__Round005_03__
#define __AbelLLExplorer__Round005_03__

#include "RoundBase.h"

USING_NS_CC;

class Actor;
class StageScene;
class Round005_03 : public RoundBase
{
public:
    Round005_03();
    virtual ~Round005_03();
    
public:
    bool initWithRoundIndex(const int,StageScene*, const Vec2& pos = Vec2::ZERO);
    static Round005_03* createWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    
public:
    virtual void celebrate(const float);
    /* 开启触摸 */
    virtual void openTouch();
    /*  回合结束回调 */
    virtual void endRound();
    
    //虫子渐变出现
    void fadeInWormsAndLeaf();
    
    //设置虫子逐个放大缩小
    void changeScaleWorms(float);
    
    //虫子改变大小完成
    void changeScaleWormsComplete();
    
    //答对题之后添加向日葵抖动gaf
    void addSunflowerGaf();
    
private:
    void cleanUp();
    virtual void onEnterRound();
    
private:
    Sprite* m_pSunflower;                            //向日葵
    Sprite* m_pWorms[5];                             //虫
    Vec2 m_wormsPos[5];                              //虫的初始化坐标
    
    Actor* m_pSunflowerActor;                        //向日葵gaf
    
    int m_effectId;
    bool m_opentouched = false;
    bool m_jumped = false;
};

#endif /* defined(__AbelLLExplorer__Round005_03__) */
