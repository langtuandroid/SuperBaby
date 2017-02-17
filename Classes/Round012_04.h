//
//  Round012_04.h
//  AbelLLExplorer
//
//  Created by Administrator on 2015/4/9.
//
//

#ifndef __AbelLLExplorer__Round012_04__
#define __AbelLLExplorer__Round012_04__

#include "RoundBase.h"
USING_NS_CC;

class Actor;
class SocialShareNode;
class StageScene;
class Round012_04 : public RoundBase
{
public:
    Round012_04();
    virtual ~Round012_04();
    
public:
    bool initWithRoundIndex(const int,StageScene*, const Vec2& pos = Vec2::ZERO);
    static Round012_04* createWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    
public:
    virtual void celebrate(const float);
    virtual void onEnterRound();
    virtual void onPreLoadedRes();
    virtual void openTouch();
    /*  回合结束回调 */
    virtual void endRound();

    //移动主角退出
    void moveActor();
    
    //主角移动完成
    void moveActorComplete(float);
    
    // 回合开场动画，小数字上升动画调度
    void numberUpSchedule(float);
    // 回合开场动画，蘑菇左移动画调度
    void mushroomsLeftMoveSchedule(float);
    // 回合开场动画，蘑菇放大缩小动画调度
    void mushroomsChangeScaleSchedule(float);
    
    //回合结束，渐隐正确的答案数字
    void fadeOutTrueNumber(float);
    
    //回合结束，渐隐正确数字的蘑菇
    void fadeOutTrueMushroom(float);
    
    //回合结束，渐隐剩下的数字和蘑菇
    void fadeOutRemainMushrroomAndNumbers(float);
    
    //添加精灵gaf
    void addSpriteGafActor();
    //回合结束分享调用
    void roundEndShareCallBack(bool isShare);
    
    //分享回调
    void shareCallback();
    
    //渐变变黑退出
    void fadeOutLayer(float);
    
private:
    void cleanUp();
    
private:
    SpriteBatchNode* m_pBN;     //BN
    Sprite* m_pMushrooms[3];    //蘑菇组
    Sprite* m_pAnswerNumbers[3];    //答案数字组
    Actor* m_pSpriteActor;          //精灵actor
    
    Vec2 m_mushroomsPos[3];         //蘑菇坐标组
    Vec2 m_answersPos[3];           //答案坐标组
    
    SocialShareNode* m_pSSNode;        // 社交分享
    
    Vec2 m_originalPoint;
    Vec2 m_touchBeganPoint;
    unsigned int m_InitAniIndex;
    unsigned int m_correctNumber;
    int m_effectId;
    
    GLubyte m_opacity = 0;
    LayerColor* m_LayerColor;
    bool m_opentouched = false;
    bool m_jumped = false;
};

#endif /* defined(__AbelLLExplorer__Round012_04__) */
