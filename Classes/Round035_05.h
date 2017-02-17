//
//  Round035_05.h
//  superBaby
//
//  Created by Administrator on 15/7/6.
//
//

#ifndef __superBaby__Round035_05__
#define __superBaby__Round035_05__

#include "RoundBase.h"
USING_NS_CC;
class Actor;
class Round035_05 : public RoundBase
{
public:
    Round035_05();
    virtual ~Round035_05();
    
public:
    bool initWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    static Round035_05* createWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    
public:
    void cleanUp();
    virtual void onEnterRound();
    virtual void celebrate(const float);
    virtual void onPreLoadedRes();
    /* 开启触摸 */
    virtual void openTouch();
    /*  回合结束回调 */
    virtual void endRound();
    
    //放大答案以提示
    void changeScaleHint();
    
    //分享回调
    void shareCallback();
    
    //回合结束分享调用
    void roundEndShareCallBack(bool isShare);
    
    void changeScaleAnswersSchedule(float);
    
    void changeScaleStonesShedule(float);
    
    //移动主角退出
    void moveActor();
    //渐变变黑退出
    void fadeOutLayer(float);
    
private:
    Sprite* m_pAnswersSp[3];            //答案图片
    Vec2 m_answersPos[3];               //答案坐标
    Vec2 m_answersEndPos[3];
    Sprite* m_pStonesSp[3];
    Vec2 m_stonesPos[3];
    int m_effectId = 0;                //音效play的id
    int m_InitAniIndex;
    // 拖放答题成功的数量
    Vec2 m_originalPoint;
    Vec2 m_touchBeganPoint;         // 拖放开始位置
    
    GLubyte m_opacity = 0;
    LayerColor* m_LayerColor;
    bool m_opentouched = false;
    bool m_jumped = false;
    Actor* m_pPaopaoActor = nullptr;
    Actor* m_pWhiteLightActor = nullptr;
    int m_curCorrectNum = 0;

};


#endif /* defined(__superBaby__Round035_05__) */
