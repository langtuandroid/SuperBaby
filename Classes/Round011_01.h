//
//  Round011_01.h
//  AbelLLExplorer
//
//  Created by Administrator on 15/4/9.
//
//

#ifndef __AbelLLExplorer__Round011_01__
#define __AbelLLExplorer__Round011_01__

#include "RoundBase.h"
#include <vector>
USING_NS_CC;

class Actor;
class StageScene;
class Round011_01 : public RoundBase
{
public:
    Round011_01();
    virtual ~Round011_01();
    
public:
    bool initWithRoundIndex(const int,StageScene*, const Vec2& pos = Vec2::ZERO);
    static Round011_01* createWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    
public:
    virtual void celebrate(const float);
    virtual void onEnterRound();
    virtual void openTouch();
    /*  回合结束回调 */
    virtual void endRound();
    //回合开始，草莓左移出现
    void strawberrysLeftMoveSchedule(float);
    //回合结束，草莓右移出现
    void strawberrysRightMoveSchedule(float);
    //选对后，改变熊actor的动作
    void changeBearActor();
    //选对后。设置草莓动作
    void changeStrawberry(Node*);
    
    //随机
    int randomIndex(int,int);
    //判断产生的随机数是否重复
    bool numberIsRepeat(std::vector<int>& ,int);
    //得到一个不重复的随机树
    int getRandomNumber(std::vector<int>&,int,int);
    
private:
    SpriteBatchNode* m_pBN;     //BN
    Sprite* m_pStrawberrys[3];  //草莓组
    Actor* m_pBearActor;        //熊actor
    Actor* m_pWaterfallActor = nullptr;
    Sprite* m_pTree;            //树

    Vec2 m_originalPoint;
    Vec2 m_touchBeganPoint;
    int m_InitAniIndex;
    std::vector<int> m_pCreatedIndexs;
    int m_effectId;
    bool m_opentouched = false;
    bool m_jumped = false;
};

#endif /* defined(__AbelLLExplorer__Round011_01__) */
