//
//  Round037_04.h
//  AbelLLExplorer
//
//  Created by Administrator on 15/5/8.
//
//

#ifndef __AbelLLExplorer__Round037_04__
#define __AbelLLExplorer__Round037_04__

#include "RoundBase.h"
#include "ui/CocosGUI.h"
USING_NS_CC;
using namespace ui;
class Actor;
class StageScene;
class Round037_04 : public RoundBase
{
public:
    Round037_04();
    virtual ~Round037_04();
    
public:
    bool initWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    static Round037_04* createWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    
public:
    void cleanUp();
    virtual void onEnterRound();
    virtual void celebrate(const float);
    virtual void onPreLoadedRes();
    /* 开启触摸 */
    virtual void openTouch();
    /*  回合结束回调 */
    virtual void endRound();
    
    //图案变大
    void graphicScaleChangeSchedule(float);
    
    //分享回调
    void shareCallback();
    
    //回合结束分享调用
    void roundEndShareCallBack(bool isShare);
    
    //移动主角退出
    void moveActor();
    //渐变变黑退出
    void fadeOutLayer(float);
    
    //过关移动门
    void rightMoveDoor();
    
    //缩小剩余图形
    void scaleChangeGraphic();
    
    //回合开始改变提示问号的大小
    void scaleChangeMark();

private:
    SpriteBatchNode* m_pBN = nullptr;
    Node* m_pOverActorNode;
    
    Sprite* m_pGraphics[3];            //图案
    
    Vec2 m_graphicsPos[3];
    
    Vec2 m_randGraphicPos[3];
    
    Sprite* m_pTargetSp;
    
    Vec2 m_targetPos;
    
    Rect m_touchRect;
    
    Vec2 m_touchCentrePos;
    
    Sprite* m_pHole;                    //门洞
    
    Vec2 m_holePos;
    
    Sprite* m_pDoor;                    //门
    
    Vec2 m_doorPos;
    
    Sprite* m_pWall;                    //墙
    
    Sprite* m_pMark;

    int m_effectId = 0;                //音效play的id
    int m_InitAniIndex;
    // 拖放答题成功的数量
    unsigned int m_numberOfdragComplete = 0;
    Vec2 m_originalPoint;
    
    Vec2 m_touchBeganPoint;         // 拖放开始位置
    
    GLubyte m_opacity = 0;
    LayerColor* m_LayerColor;
};

#endif /* defined(__AbelLLExplorer__Round037_04__) */
