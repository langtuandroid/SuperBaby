//
//  Round007_01.h
//  AbelLLExplorer
//
//  Created by Administrator on 15/4/3.
//
//

#ifndef __AbelLLExplorer__Round007_01__
#define __AbelLLExplorer__Round007_01__

#include "RoundBase.h"
USING_NS_CC;

class Actor;
class StageScene;
class Round007_01 : public RoundBase
{
public:
    Round007_01();
    virtual ~Round007_01();
    
public:
    bool initWithRoundIndex(const int,StageScene*, const Vec2& pos = Vec2::ZERO);
    static Round007_01* createWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    
public:
    virtual void celebrate(const float);
    virtual void onPreLoadedRes();
    virtual void openTouch();
    /*  回合结束回调 */
    virtual void endRound();
    //答对后下移
    void areasDown();
    //
    void scaleChangeSchedule(float);
    
private:
    void cleanUp();
    virtual void onEnterRound();
        
private:
    SpriteBatchNode* m_pBN;     //BN
    Sprite* m_pAnimals[3];          //动物
    Sprite* m_pBlackAnimals[3];     //黑色动物阴影
    Sprite* m_pCyanFrames[3];       //青色背景框
    Sprite* m_pMarks[3];            //问号组
    Actor* m_pAnimalActors[3];      //动物actors
    
    Vec2 m_animalsPos[3];           //动物坐标组
    Vec2 m_blackAnimalsPos[3];      //黑色动物阴影坐标组
    Vec2 m_cyanFramesPos[3];        //青色背景框坐标组
    Vec2 m_marksPos[3];             //问号坐标组
    Vec2 m_gafPos[3];               //gaf坐标
    
    Vec2 m_originalPoint;
    Vec2 m_touchBeganPoint;
    int m_correctNumber;
    int m_effectId;
    Vec2 m_originalPoints[3];
    int m_InitAniIndex = 0;
    bool m_opentouched = false;
    bool m_jumped = false;
};

#endif /* defined(__AbelLLExplorer__Round007_01__) */
