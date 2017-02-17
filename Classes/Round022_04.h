//
//  Round022_04.h
//  AbelLLExplorer
//
//  Created by Administrator on 15/5/22.
//
//

#ifndef __AbelLLExplorer__Round022_04__
#define __AbelLLExplorer__Round022_04__

#include "RoundBase.h"

USING_NS_CC;

class Actor;
class StageScene;
class Round022_04 : public RoundBase
{
public:
    Round022_04();
    virtual ~Round022_04();
    
public:
    bool initWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    static Round022_04* createWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    
public:
    void cleanUp();
    virtual void onEnterRound();
    virtual void celebrate(const float);
    /* 开启触摸 */
    virtual void openTouch();
    /*  回合结束回调 */
    virtual void endRound();
    //分享回调
    void shareCallback();
    
    //回合结束分享调用
    void roundEndShareCallBack(bool isShare);
    
    /*  处理npc收集 */
    virtual void processNpcCollect();
    
    //移动主角退出
    void moveActor();
    //渐变变黑退出
    void fadeOutLayer(float);
    
    //答对后改变actor动作
    void changeActor();

private:
    
    Actor* m_pEggActor = nullptr;
    Actor* m_pDinoActor = nullptr;
    
    Rect m_answersRect[3];
    
    int m_InitAniIndex;
    int m_effectId = 0;                //音效play的id
    GLubyte m_opacity = 0;
    LayerColor* m_LayerColor;
};

#endif /* defined(__AbelLLExplorer__Round022_04__) */
