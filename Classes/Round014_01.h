//
//  Round014_01.h
//  AbelLLExplorer
//
//  Created by Administrator on 15/5/12.
//
//

#ifndef __AbelLLExplorer__Round014_01__
#define __AbelLLExplorer__Round014_01__

#include "RoundBase.h"
#include "ui/CocosGUI.h"
USING_NS_CC;
using namespace ui;

class Actor;
class StageScene;
class Round014_01 : public RoundBase
{
public:
    Round014_01();
    virtual ~Round014_01();
    
public:
    bool initWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    static Round014_01* createWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    
public:
    void cleanUp();
    virtual void onEnterRound();
    virtual void celebrate(const float);
    /* 开启触摸 */
    virtual void openTouch();
    /*  回合结束回调 */
    virtual void endRound();
    
    //依次改变碎片大小
    void scaleChangeSchedule(float);
    
    //渐变显隐黄色目标块
    void fadeYellowPiece(float);
    
    //渐变隐藏剩余碎片
    void fadeOutPieces();
    
private:
    SpriteBatchNode* m_pBN = nullptr;
    
    Sprite* m_pBeforeSp;            //前景图
    Sprite* m_pPieces[3];           //碎片
    
    Sprite* m_pYellowPiece;         //黄色提示碎片
    
    ImageView* m_pTargetImageView;            //目标sp
    
    Vec2 m_piecesPos[3];               //碎片坐标
    Vec2 m_piecesEndPos[3];            //碎片随机后最终坐标
    
    int m_InitAniIndex;
    Vec2 m_originalPoint;
    Vec2 m_touchBeganPoint;         // 拖放开始位置
    int m_effectId = 0;                //音效play的id
    bool m_opentouched = false;
    bool m_jumped = false;
};

#endif /* defined(__AbelLLExplorer__Round014_01__) */
