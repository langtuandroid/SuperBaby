//
//  Round011_02.h
//  AbelLLExplorer
//
//  Created by Administrator on 15/4/9.
//
//

#ifndef __AbelLLExplorer__Round011_02__
#define __AbelLLExplorer__Round011_02__

#include "RoundBase.h"
#include <vector>
USING_NS_CC;

class Actor;
class StageScene;
class Round011_02 : public RoundBase
{
public:
    Round011_02();
    virtual ~Round011_02();
    
public:
    bool initWithRoundIndex(const int,StageScene*, const Vec2& pos = Vec2::ZERO);
    static Round011_02* createWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    
public:
    virtual void celebrate(const float);
    virtual void onEnterRound();
    virtual void onPreLoadedRes();
    virtual void openTouch();
    /*  回合结束回调 */
    virtual void endRound();

    //回合开始，下降木桥和拼图板
    void downBridgeAndPuzzleBoard();
    //拼图块上升
    void upPuzzlePieceSchedule(float);
    
    //设置拼图板红色提示块循环显隐
    void redHintPuzzleVisible(float);
    
    //答对后延时拼图块下降
    void delayDownPuzzlePiece(float);
    
    //答对后拼图块下降
    void downPuzzlePieceSchedule(float);
    
    //答对后拼图板渐变消失
    void fadeOutPuzzleBoard(float);
    
    //答对后木桥第二次下降
    void downSecondBridge();
 
    // 主角行走到桥
    void actorMoveToBridge();
    
    //随机
    int randomIndex(int, int);
    //判断产生的随机数是否重复
    bool numberIsRepeat(std::vector<int>& ,int);
    //得到一个不重复的随机树
    int getRandomNumber(std::vector<int>&,int,int);
    
private:
    SpriteBatchNode* m_pSBN = nullptr;     // SBN
    Sprite* m_pPuzzles[3];      // 拼图块组
    Sprite* m_pPuzzleBoard[3];  // 拼图板（包括完整状态和不完整状态以及红色提示）
    Sprite* m_pBridge;          // 木桥
    
    Rect m_rect = Rect(1030.0f, 690.0f, 219.0f, 219.0f);                // 触摸响应区域

    Vec2 m_originalPoint;
    Vec2 m_touchBeganPoint;
    unsigned int m_InitAniIndex = 0;
    bool m_bIsVisible = false;             //用于设置提示拼图块显隐
    int m_effectId;
    std::vector<int> m_pCreatedIndexs;
    Vec2 m_originalPoints[3];
    Actor* m_pWaterfallActor = nullptr;
    bool m_opentouched = false;
    bool m_jumped = false;
};

#endif /* defined(__AbelLLExplorer__Round011_02__) */
