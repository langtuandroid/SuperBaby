//
//  Round036_04.h
//  superBaby
//
//  Created by Administrator on 15/7/7.
//
//

#ifndef __superBaby__Round036_04__
#define __superBaby__Round036_04__

#include "RoundBase.h"
#include <ui/CocosGUI.h>

USING_NS_CC;
class Actor;
class Round036_04 : public RoundBase
{
public:
    Round036_04();
    virtual ~Round036_04();
    
public:
    bool initWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    static Round036_04* createWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    
public:
    void cleanUp();
    virtual void onEnterRound();
    virtual void celebrate(const float);
    virtual void onPreLoadedRes();
    /* 开启触摸 */
    virtual void openTouch();
    /*  回合结束回调 */
    virtual void endRound();
    
    //放大题目以提示
    void changeScaleHintSchedule(float);
    
    //分享回调
    void shareCallback();
    
    //回合结束分享调用
    void roundEndShareCallBack(bool isShare);
    
    void changeScaleAnswersSchedule(float);
    
    //渐变变黑退出
    void fadeOutLayer(float);
    
    //答对改变ms
    void changeActor();
    
    //隐藏答案
    void fadeAnswers();
    
private:
    Sprite* m_pAnswersSp[4];            //答案图片
    Vec2 m_answersPos[4];               //答案坐标
    Vec2 m_answersEndPos[4];
    ui::ImageView* m_pTopicImageView[4];
    Vec2 m_topicsPos[4];
    Actor* m_pMSActor = nullptr;        //ms
    int m_effectId = 0;                //音效play的id
    int m_InitAniIndex;
    // 拖放答题成功的数量
    Vec2 m_originalPoint;
    Vec2 m_touchBeganPoint;         // 拖放开始位置
    
    GLubyte m_opacity = 0;
    LayerColor* m_LayerColor;
    bool m_opentouched = false;
    bool m_jumped = false;
    Actor* m_pBoguangActor = nullptr;
    Actor* m_pPaopaoActor = nullptr;
    Actor* m_pWhiteLightActor = nullptr;
    int m_curCorrectNum = 0;
};


#endif /* defined(__superBaby__Round036_04__) */
