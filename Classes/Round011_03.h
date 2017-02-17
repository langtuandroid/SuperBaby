//
//  Round011_03.h
//  AbelLLExplorer
//
//  Created by Administrator on 15/4/9.
//
//

#ifndef __AbelLLExplorer__Round011_03__
#define __AbelLLExplorer__Round011_03__

#include "RoundBase.h"
#include <vector>
#include <time.h>
USING_NS_CC;

class Actor;
class StageScene;
class Round011_03 : public RoundBase
{
public:
    Round011_03();
    virtual ~Round011_03();
    
public:
    bool initWithRoundIndex(const int,StageScene*, const Vec2& pos = Vec2::ZERO);
    static Round011_03* createWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    
public:
    virtual void celebrate(const float);
    virtual void onEnterRound();
    virtual void onPreLoadedRes();
    virtual void openTouch();
    /*  回合结束回调 */
    virtual void endRound();
    //回合开始，依次显示工具
    void showToolsSchedule(float);
    
    //答对后改变相应actor的动作
    void changeAction(Actor*);
    
    //离开回合，缩小actor
    void changeActors(float);
    
    //随机
    int randomIndex(int,int);
    //判断产生的随机数是否重复
    bool numberIsRepeat(std::vector<int>& ,int);
    //得到一个不重复的随机树
    int getRandomNumber(std::vector<int>&,int,int);
    
private:
    void cleanUp();
    
private:
    SpriteBatchNode* m_pBN;     //BN
    Sprite* m_pTools[3];        //工具组
    Actor* m_pOfficeworkers[3];   //职员actor组
    
    Vec2 m_officeworkersPos[3];     //职员位置组
    
    Vec2 m_toolsPos[3];             //工具位置组
    
    Rect m_rect[3];                 //触摸响应区
    Vec2 m_centerRectsPos[3];            //触摸区中点坐标组

    Vec2 m_originalPoint;
    Vec2 m_touchBeganPoint;
    unsigned int m_InitAniIndex;
    unsigned int m_correctNumber;
    std::vector<int> m_pCreatedIndexs;
    int m_effectId;
    Vec2 m_originalPoints[3];
    bool m_opentouched = false;
    bool m_jumped = false;
};

#endif /* defined(__AbelLLExplorer__Round011_03__) */
