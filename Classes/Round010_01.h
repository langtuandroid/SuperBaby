//
//  Round010_01.h
//  AbelLLExplorer
//
//  Created by Administrator on 15/4/8.
//
//

#ifndef __AbelLLExplorer__Round010_01__
#define __AbelLLExplorer__Round010_01__

#include "RoundBase.h"
USING_NS_CC;

class NpcActor;
class StageScene;
class Round010_01 : public RoundBase
{
public:
    Round010_01();
    virtual ~Round010_01();
    
public:
    bool initWithRoundIndex(const int,StageScene*, const Vec2& pos = Vec2::ZERO);
    static Round010_01* createWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    
public:
    virtual void celebrate(const float);
    virtual void onEnterRound();
    virtual void onPreLoadedRes();
    virtual void openTouch();
    /*  回合结束回调 */
    virtual void endRound();
    /*  处理npc收集 */
    virtual void processNpcCollect();
    // 回合开场动画，渐变出现照片动画调度
    void fadeInPhotosUpSchedule(float);
    // 回合结束动画，渐变隐藏照片动画调度
    void fadeOutPhotosUpSchedule(float);
    
private:
    SpriteBatchNode* m_pBN = nullptr;
    NpcActor* m_pNpcActor = nullptr;     //照相机Actor
    Sprite* m_pLeftBackground = nullptr;  //左边背景图
    Sprite* m_pPhotos[3];       //照片组
    
    Vec2 m_photosPos[3];        //照片坐标组

    Vec2 m_originalPoint;
    Vec2 m_touchBeganPoint;
    unsigned int m_InitAniIndex = 0;
    int m_effectId = 0;
    bool m_opentouched = false;
    bool m_jumped = false;
};

#endif /* defined(__AbelLLExplorer__Round010_01__) */
