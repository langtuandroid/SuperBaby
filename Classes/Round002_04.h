//
//  Round002_04.h
//  AbelLLExplorer
//
//  Created by adinnet on 15/3/25.
//
//  第二关第四回合
//

#ifndef __AbelLLExplorer__Round002_04__
#define __AbelLLExplorer__Round002_04__

#include "RoundBase.h"

USING_NS_CC;

class SocialShareNode;
class Actor;
class StageScene;
class Round002_04 : public RoundBase
{
public:
    Round002_04();
    virtual ~Round002_04();
    
public:
    bool initWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    static Round002_04* createWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    
public:
    virtual void onEnterRound();
    virtual void celebrate(const float);
    virtual void onPreLoadedRes();
    /*  回合结束回调 */
    virtual void endRound();
    /* 是否本关卡最后一个回合 */
    virtual bool isLastRound() const {return true;}
    /* 开启触摸 */
    virtual void openTouch();

public:
    void basketUpComplete();
    //回合结束分享调用
    void roundEndShareCallBack(bool isShare);
    
    //分享回调
    void shareCallback();
    
private:
    /* 篮子上升 */
    void basketUp();
    
private:
    SpriteBatchNode* m_pBN = nullptr;       // BN
    SocialShareNode* m_pSSNode = nullptr;   // 社交分享
    
    Sprite* m_pTargetSp = nullptr;
    Vec2 m_touchBeganPoint;
    Sprite* m_eggSps[3];
    Vec2 m_originalPoints[3];
    unsigned int m_correctNumber = 0;
    int m_effectId = 0;
};


#endif /* defined(__AbelLLExplorer__Round002_04__) */
