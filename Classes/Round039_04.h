//
//  Round039_04.h
//  superBaby
//
//  Created by Administrator on 15/7/23.
//
//

#ifndef __superBaby__Round039_04__
#define __superBaby__Round039_04__

#include "RoundBase.h"
#include <ui/CocosGUI.h>
USING_NS_CC;
class Actor;
class Round039_04 : public RoundBase
{
public:
    Round039_04();
    virtual ~Round039_04();
    
public:
    bool initWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    static Round039_04* createWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    
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
    
    //改变提示问号大小
    void changeScaleHintsSchedule(float);
    
    //上升答案
    void upAnswersSchedule(float);
    
    void fadeOutAnswers();
    
    void changeActor();
    
    void moveLeadActor();
    
    void stageOver(float);
    
private:
    Sprite* m_pAnswersSp[3];  //答案图片
    Vec2 m_answersPos[3];               //答案坐标
    Vec2 m_answersEndPos[3];
    ui::ImageView* m_pHintsImageView[2];
    Vec2 m_hintsPos[2];
    ui::ImageView* m_pTopicImageView = nullptr;       //题目
    int m_effectId = 0;                //音效play的id
    int m_InitAniIndex = 0;
    GLubyte m_opacity = 0;
    LayerColor* m_LayerColor;
    bool m_opentouched = false;
    bool m_jumped = false;
    Actor* m_pMSActor = nullptr;
    Vec2 m_originalPoint;
    Vec2 m_touchBeganPoint;         // 拖放开始位置
    unsigned int m_curCorrectNum = 0;
    unsigned int m_firstNum = 0;        //up 1  down 2 init 0
    Sprite* m_pScrewSp = nullptr;
    Sprite* m_pDoorSp = nullptr;
    Sprite* m_pOverSp = nullptr;
};


#endif /* defined(__superBaby__Round039_04__) */
