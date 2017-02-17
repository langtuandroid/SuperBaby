//
//  Round025_03.h
//  AbelLLExplorer
//
//  Created by Administrator on 15/5/6.
//
//

#ifndef __AbelLLExplorer__Round025_03__
#define __AbelLLExplorer__Round025_03__

#include "RoundBase.h"

USING_NS_CC;

class Actor;
class StageScene;
class Round025_03 : public RoundBase
{
public:
    Round025_03();
    virtual ~Round025_03();
    
public:
    bool initWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    static Round025_03* createWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    
public:
    void cleanUp();
    virtual void onEnterRound();
    virtual void celebrate(const float);
    /* 开启触摸 */
    virtual void openTouch();
    /*  回合结束回调 */
    virtual void endRound();
    
    //回合开始改变树的尺寸
    void scaleChangeTreeSchedule(float);
    
private:
    SpriteBatchNode* m_pBN = nullptr;

    Sprite* m_pTrees[3];                //树
    Sprite* m_pTempTrees[3];
    Image* m_pTreesImages[3];           //树小图组
    Image* m_pTempTreesImages[3];
    Vec2 m_treesPos[3];                 //树的坐标
    int m_effectId = 0;                //音效play的id
    int m_curSelIndex;                 //选中树索引
    int m_InitAniIndex = 0;
    Actor* m_pBoguangActor = nullptr;

};

#endif /* defined(__AbelLLExplorer__Round025_03__) */
