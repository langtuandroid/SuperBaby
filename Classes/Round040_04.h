//
//  Round040_04.h
//  superBaby
//
//  Created by Administrator on 15/7/24.
//
//

#ifndef __superBaby__Round040_04__
#define __superBaby__Round040_04__

#include "RoundBase.h"
#include <ui/CocosGUI.h>
USING_NS_CC;
class Actor;
class Round040_04 : public RoundBase
{
public:
    Round040_04();
    virtual ~Round040_04();
    
public:
    bool initWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    static Round040_04* createWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    
public:
    void cleanUp();
    virtual void onEnterRound();
    virtual void celebrate(const float);
    virtual void onPreLoadedRes();
    /* 开启触摸 */
    virtual void openTouch();
    /*  回合结束回调 */
    virtual void endRound();
    
    //分享回调
    void shareCallback();
    
    //回合结束分享调用
    void roundEndShareCallBack(bool isShare);
    
    //渐变变黑退出
    void fadeOutLayer(float);
    
    //改变答案大小
    void changeScaleAnswersSchedule(float);
    //改变题目大小
    void changeScaleTopicSchedule(float);
    
    void fadeOutAnswers();
        
    void moveLeadActor();
    
    void stageOver(float);
    
private:
    Sprite* m_pAnswersSp[3];  //答案图片
    Vec2 m_answersPos[3];               //答案坐标
    Vec2 m_answersEndPos[3];
    Sprite* m_pTopicsSp[2];
    Vec2 m_topicsPos[2];
    ui::ImageView* m_pTargetsImageView[2];
    Vec2 m_targetsPos[2];
    int m_effectId = 0;                //音效play的id
    int m_InitAniIndex = 0;
    GLubyte m_opacity = 0;
    LayerColor* m_LayerColor;
    bool m_opentouched = false;
    bool m_jumped = false;
    Vec2 m_originalPoint;
    Vec2 m_touchBeganPoint;         // 拖放开始位置
    unsigned int m_curCorrectNum = 0;
    Sprite* m_pStageSp = nullptr;
};

#endif /* defined(__superBaby__Round040_04__) */
