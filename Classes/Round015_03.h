//
//  Round015_03.h
//  AbelLLExplorer
//
//  Created by Administrator on 15/5/14.
//
//

#ifndef __AbelLLExplorer__Round015_03__
#define __AbelLLExplorer__Round015_03__

#include "RoundBase.h"

USING_NS_CC;

class Actor;
class StageScene;
class Round015_03 : public RoundBase
{
public:
    Round015_03();
    virtual ~Round015_03();
    
public:
    bool initWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    static Round015_03* createWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    
public:
    void cleanUp();
    virtual void onEnterRound();
    virtual void celebrate(const float);
    /* 开启触摸 */
    virtual void openTouch();
    /*  回合结束回调 */
    virtual void endRound();
    
    //出现花图片
    void appearSunflowersSchedule(float);
    
    //答对后，花图片动作
    void chooseTrueSunflowersActions();
    
    //答对后改变飞船动作
    void changeActor();
    
    //渐隐魔法池
    void fadeOutMagicPool();
    
private:
    SpriteBatchNode* m_pBN = nullptr;
    Actor* m_pFlyShipActor;                     //飞船
    Actor* m_pMagicActor;                       //魔法效果gaf
    
    Sprite* m_pSunflowers[4];                    //向日葵
    
    Vec2 m_sunflowersPos[4];
    
    Vec2 m_sunflowersEndPos[4];                 //随机后的坐标
    
    Vec2 m_sunflowerMovePos[4];                 //花最后聚拢的位置
    Vec2 m_sunflowerMoveEndPos[4];              //随机后花最后聚拢的位置
    
    Vec2 m_sunflowerMoveRelativePos[4];         //花最后聚拢的相对位移
    Vec2 m_sunflowerMoveRelativeEndPos[4];      //随机后花最后聚拢的相对位移
    
    Sprite* m_pMagicPool;                       //魔法池
    Vec2 m_hintsPos[3];
    
    int m_InitAniIndex;
    int m_effectId = 0;                        //音效play的id
    int m_curCorrectNumber = 0;                //当前答对个数
    unsigned int m_curSameBallRandNum = 0;       //当前相同球的随机编号
    bool m_opentouched = false;
    bool m_jumped = false;

};

#endif /* defined(__AbelLLExplorer__Round015_03__) */
