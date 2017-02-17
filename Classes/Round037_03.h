//
//  Round037_03.h
//  AbelLLExplorer
//
//  Created by Administrator on 15/5/8.
//
//

#ifndef __AbelLLExplorer__Round037_03__
#define __AbelLLExplorer__Round037_03__

#include "RoundBase.h"

USING_NS_CC;

class Actor;
class StageScene;
class Round037_03 : public RoundBase
{
public:
    Round037_03();
    virtual ~Round037_03();
    
public:
    bool initWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    static Round037_03* createWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    
public:
    void cleanUp();
    virtual void onEnterRound();
    virtual void celebrate(const float);
    /* 开启触摸 */
    virtual void openTouch();
    /*  回合结束回调 */
    virtual void endRound();
    /* 播放过关引导 */
    void playPassActor();
    
private:
    SpriteBatchNode* m_pBN = nullptr;
    
    Sprite* m_pBirdEggs[4];             //鸟蛋
    
    Vec2 m_birdEggsPos[4];              //鸟蛋位置
    
    Sprite* m_pBirdNestOne[3];             //鸟窝一
    Sprite* m_pBirdNestTwo[3];             //鸟窝二
    
    Vec2 m_birdNestsPos[2];
    
    int m_effectId = 0;                //音效play的id
    unsigned int m_curCorrectNumber;    //当前答对个数
    Vec2 m_originalPoint;
    
    Vec2 m_touchBeganPoint;         // 拖放开始位置
    
    Rect m_birdNestRect[2];
    
    unsigned int m_birdNestOneEggNum;
    unsigned int m_birdNestTwoEggNum;
    GLubyte m_opacity = 0;
    LayerColor* m_LayerColor;
};

#endif /* defined(__AbelLLExplorer__Round037_03__) */
