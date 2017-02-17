//
//  Round001_06.h
//  AbelLLExplorer
//
//  Created by Administrator on 2015/03/27.
//
//

#ifndef __AbelLLExplorer__Round001_06__
#define __AbelLLExplorer__Round001_06__

#include "RoundBase.h"

USING_NS_CC;

class Actor;
class SocialShareNode;
class StageScene;
class Round001_06 : public RoundBase
{
public:
    Round001_06();
    virtual ~Round001_06();
    
public:
    bool initWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    static Round001_06* createWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    
public:
    virtual void onEnterRound();
    virtual void celebrate(const float);
    virtual void onPreLoadedRes();
    /* 开启触摸 */
    virtual void openTouch();
    /*  回合结束回调 */
    virtual void endRound();
    
    //隐藏拼图
    void fadeOutPuzzle();
    
    //切换蛇的动作
    void changeSnakeAction();
    
    //回合结束分享调用
    void roundEndShareCallBack(bool isShare);
    
    //分享回调
    void shareCallback();
    
    //渐变变黑退出
    void fadeOutLayer(float);
    
private:
    SpriteBatchNode* m_pPuzzleNode = nullptr;   // 拼图块
    Actor* m_pSnakeActor = nullptr;             // 蛇 actor
    
    // 拼图填补选择块
    Sprite* m_pPuzzleChoosePiece[3] = {nullptr, nullptr, nullptr};
    Sprite* m_pPuzzleBoard = nullptr;           // 拼图板
    Sprite* m_pPuzzleBoardFull = nullptr;       // 完整拼图
    Rect m_pPuzzleRect;                         // 需要拼图的区域
    
    Vec2 m_originalPoint = Vec2::ZERO;
    Vec2 m_touchBeganPoint = Vec2::ZERO;
    
    Vec2 m_originalPoints[3];
    
    unsigned int m_correctNumber = 0;
    
    int m_effectId = 0;                //音效play的id
    GLubyte m_opacity = 0;
    LayerColor* m_LayerColor = nullptr;
};

#endif /* defined(__AbelLLExplorer__Round001_06__) */
