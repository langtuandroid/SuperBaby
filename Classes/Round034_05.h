//
//  Round034_05.h
//  superBaby
//
//  Created by Administrator on 15/7/2.
//
//

#ifndef __superBaby__Round034_05__
#define __superBaby__Round034_05__

#include "RoundBase.h"
#include <ui/CocosGUI.h>
USING_NS_CC;
class Actor;
class Round034_05 : public RoundBase
{
public:
    Round034_05();
    virtual ~Round034_05();
    
public:
    bool initWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    static Round034_05* createWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    
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
    void changeScaleHintSchedule(float);
    
    //分享回调
    void shareCallback();
    
    //回合结束分享调用
    void roundEndShareCallBack(bool isShare);
    
    void changeScaleAnswersSchedule(float);
    
    //移动主角退出
    void moveActor();
    //渐变变黑退出
    void fadeOutLayer(float);
    
    //答对旋转齿轮
    void RotateWheel();

    //隐藏剩余答案
    void fadeAnswers();
    
    //答对后开门
    void openDoor();
    
private:
    ui::ImageView* m_pTopicsImageView[2];
    Vec2 m_topicsPos[2];
    ui::ImageView* m_pAnswersImageView[3];            //答案图片
    Vec2 m_answersPos[3];               //答案坐标
    Vec2 m_answersEndPos[3];
    Sprite* m_pScrewSp = nullptr;
    Sprite* m_pDoorSp = nullptr;
    Sprite* m_pShadowSp = nullptr;
    Sprite* m_pOverSp = nullptr;
    int m_effectId = 0;                //音效play的id
    int m_InitAniIndex = 0;
    // 拖放答题成功的数量
    Vec2 m_originalPoint;
    Vec2 m_touchBeganPoint;         // 拖放开始位置
    
    GLubyte m_opacity = 0;
    LayerColor* m_LayerColor;
    bool m_opentouched = false;
    bool m_jumped = false;
    Actor* m_pPaopaoActor = nullptr;
    Actor* m_pWhiteLightActor = nullptr;
};


#endif /* defined(__superBaby__Round034_05__) */
