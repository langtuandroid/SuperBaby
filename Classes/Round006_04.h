//
//  Round006_04.h
//  AbelLLExplorer
//
//  Created by Administrator on 15/4/2.
//
//

#ifndef __AbelLLExplorer__Round006_04__
#define __AbelLLExplorer__Round006_04__

#include "RoundBase.h"
#include <ui/CocosGUI.h>
USING_NS_CC;
using namespace ui;
class Actor;
class SocialShareNode;
class StageScene;
class Round006_04 : public RoundBase
{
public:
    Round006_04();
    virtual ~Round006_04();
    
public:
    bool initWithRoundIndex(const int,StageScene*, const Vec2& pos = Vec2::ZERO);
    static Round006_04* createWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    
public:
    virtual void celebrate(const float);
    virtual void onPreLoadedRes();
    /* 开启触摸 */
    virtual void openTouch();
    /*  回合结束回调 */
    virtual void endRound();
    
    //改变主角动作，跳上风车
    void changeActor();
    
    //主角跳跃完成
    void changeActorComplete();
    
    //出场移动颜色板
    void moveBoardUp();
    
    //颜色节点依次放大缩小
    void changeScaleColorNodes();
    
    //显示提示区域
    void fadeHint(float);
    
    //答对题后旋转风车并移动篮子
    void answerComplete(float);
    
    //移动篮子
    void moveBasket();

    //回合结束分享调用
    void roundEndShareCallBack(bool isShare);
    
    //分享回调
    void shareCallback();
    
    //渐变变黑退出
    void fadeOutLayer(float);
    
private:
    void cleanUp();
    virtual void onEnterRound();
    
private:
    SpriteBatchNode* m_pWindmillBN;     //风车BN文件
    SpriteBatchNode* m_pColorBN;        //颜色BN文件
    Sprite* m_pColors[4];               //颜色组
    Sprite* m_pBasketBack;              //篮子后面部分
    Sprite* m_PBasketFront;             //篮子前面部分
    Sprite* m_pColorBoard;              //颜色底板
    ImageView* m_pWindmill[3];             //风车组
    Sprite* m_pHintSp = nullptr;
    Sprite* m_pSticks[3];               //风车棒组
    Vec2 m_colorsPos[4];                 //颜色位置组
    Vec2 m_colorsEndPos[4];
    Vec2 m_windmillsPos[3];             //风车位置组
    Vec2 m_sticksPos[3];                //风车棒位置组
    SocialShareNode* m_pSSNode;        // 社交分享
    
    Node* m_pNode;
    int m_effectId;
    GLubyte m_opacity = 0;
    LayerColor* m_LayerColor;
    Vec2 m_originalPoint;
    Vec2 m_touchBeganPoint;
    bool m_opentouched = false;
    bool m_jumped = false;
};

#endif /* defined(__AbelLLExplorer__Round006_04__) */
