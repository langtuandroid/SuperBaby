//
//  Round014_03.h
//  AbelLLExplorer
//
//  Created by Administrator on 2015/05/12.
//
//

#ifndef __AbelLLExplorer__Round014_03__
#define __AbelLLExplorer__Round014_03__

#include "RoundBase.h"

class Actor;
class StageScene;
namespace cocos2d {
    namespace ui {
        class ImageView;
    }
}

class Round014_03 : public RoundBase
{
public:
    Round014_03();
    virtual ~Round014_03();
    
public:
    bool initWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    static Round014_03* createWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    
public:
    virtual void onEnterRound();
    virtual void celebrate(const float);
    /* 开启触摸 */
    virtual void openTouch();
    /*  回合结束回调 */
    virtual void endRound();
    /* 依次改变飞船大小 */
    void scaleChangeSchedule(float);
    
    /* 延伸玻璃桥 */
    void moveGlassBoard();
    
    /* 改变红色灯的动作 */
    void changeActor();
    
private:
    Sprite* m_pGlassBoard = nullptr;            // 玻璃板

    cocos2d::ui::ImageView* m_pAirShips[4];     // 飞船
    Vec2 m_airShipsPos[4];
    Actor* m_pRedLightsActorUp = nullptr;       // 上部的红色灯actor
    Actor* m_pRedLightsActorDown = nullptr;     // 下部的红色灯actor
    
    int m_InitAniIndex = 0;
    int m_effectId = 0;                         // 音效play的id
    unsigned int m_curSameBallRandNum = 0;      // 当前相同球的随机编号
    unsigned int m_curCorrectNumber = 0;
    int m_finishedNumber = -1;                  // 已经完成的补充
    unsigned int m_number = 0;
    bool m_opentouched = false;
    bool m_jumped = false;
};

#endif /* defined(__AbelLLExplorer__Round014_03__) */
