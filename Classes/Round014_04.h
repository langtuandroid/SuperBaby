//
//  Round014_04.h
//  AbelLLExplorer
//
//  Created by Administrator on 15/5/12.
//
//

#ifndef __AbelLLExplorer__Round014_04__
#define __AbelLLExplorer__Round014_04__

#include "RoundBase.h"

USING_NS_CC;

class Actor;
class StageScene;
class Round014_04 : public RoundBase
{
public:
    Round014_04();
    virtual ~Round014_04();
    
public:
    bool initWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    static Round014_04* createWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    
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
    
    //移动主角退出
    void moveActor();
    //渐变变黑退出
    void fadeOutLayer(float);
    //答案上升
    void upAnswersNumberSchedule(float);
    
    //渐变问号以提示
    void scaleChangeMarkSchedule(float);
    
    //改变答案大小
    void scaleChangeSchedule(float);
    
    //改变actor
    void changeActor();
    
    //下降飞梯
    void downFlyLadder();
    
    //飞船上升
    void airShipUp();
    
    //隐藏剩余答案
    void fadeOutAnswers();
    
private:
    Actor* m_pReelActor;            //转轮
    
    Sprite* m_pTargetMarkSp;            //问号
    
    Sprite* m_pAnswersNumber[3];        //答案数字
    Sprite* m_pAirSp;                   //飞船
    
    Vec2 m_answerNumbersPos[3];
    Vec2 m_answerNumbersEndPos[3];
    
    int m_InitAniIndex;
    Vec2 m_originalPoint;
    Vec2 m_touchBeganPoint;         // 拖放开始位置
    int m_effectId = 0;                //音效play的id
    GLubyte m_opacity = 0;
    LayerColor* m_LayerColor;
    bool m_opentouched = false;
    bool m_jumped = false;
};

#endif /* defined(__AbelLLExplorer__Round014_04__) */
