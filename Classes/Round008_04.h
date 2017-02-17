//
//  Round008_04.h
//  AbelLLExplorer
//
//  Created by Administrator on 2015/4/7.
//
//

#ifndef __AbelLLExplorer__Round008_04__
#define __AbelLLExplorer__Round008_04__

#include "RoundBase.h"
USING_NS_CC;

class Actor;
class SocialShareNode;
class StageScene;
class Round008_04 : public RoundBase
{
public:
    Round008_04();
    virtual ~Round008_04();
    
public:
    bool initWithRoundIndex(const int,StageScene*, const Vec2& pos = Vec2::ZERO);
    static Round008_04* createWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    
public:
    virtual void celebrate(const float);
    virtual void onEnterRound();
    virtual void onPreLoadedRes();
    virtual void openTouch();
    /*  回合结束回调 */
    virtual void endRound();

    //移动主角退出
    void moveActor();

    //回合结束分享调用
    void roundEndShareCallBack(bool isShare);
    
    //分享回调
    void shareCallback();
    
    //渐变变黑退出
    void fadeOutLayer(float);
private:
    void cleanUp();
    void addListener();
    
private:
    Sprite* m_numberSp[3];
    SpriteBatchNode* m_pSBN;
    SocialShareNode* m_pSSNode;
    Node* m_pOverActorNode;
    Sprite* m_pTree7Sp;
    int m_correctNumber;
    Actor* m_pGoatActor;
    int m_effectId;
    GLubyte m_opacity = 0;
    LayerColor* m_LayerColor;
};

#endif /* defined(__AbelLLExplorer__Round008_04__) */
