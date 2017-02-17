//
//  Round026_04.h
//  AbelLLExplorer
//
//  Created by Administrator on 15/6/1.
//
//

#ifndef __AbelLLExplorer__Round026_04__
#define __AbelLLExplorer__Round026_04__

#include "RoundBase.h"
#include "ui/CocosGUI.h"
USING_NS_CC;
class Actor;
class StageScene;
class Round026_04 : public RoundBase
{
public:
    Round026_04();
    virtual ~Round026_04();
    
public:
    bool initWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    static Round026_04* createWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    
public:
    void cleanUp();
    virtual void onEnterRound();
    virtual void celebrate(const float);
    virtual void onPreLoadedRes();
    /* 开启触摸 */
    virtual void openTouch();
    /*  回合结束回调 */
    virtual void endRound();
    
    //上升答案
    void upMoveAnswersSchedule(float);
    
    //放大答案以提示
    void changeScaleTopic();
    
    //答对后下降答案
    void downMoveAnswersTopicSchedule(float);
    
    //分享回调
    void shareCallback();
    
    //回合结束分享调用
    void roundEndShareCallBack(bool isShare);
    
    //移动主角退出
    void moveActor();
    //渐变变黑退出
    void fadeOutLayer(float);
    
    //答对改变ms
    void changeActor();
    
    //打开门
    void openDoor();
    
    //隐藏剩余答案
    void fadeAnswers();
private:
    Sprite* m_pAnswersSp[3];            //答案图片
    Vec2 m_answersPos[3];               //答案坐标
    Vec2 m_answersEndPos[3];
    ui::ImageView* m_pTopicImageView = nullptr;       //题目
    Actor* m_pMSActor = nullptr;        //ms
    int m_effectId = 0;                //音效play的id
    int m_InitAniIndex;
    // 拖放答题成功的数量
    unsigned int m_numberOfdragComplete = 0;
    Vec2 m_originalPoint;
    
    Vec2 m_touchBeganPoint;         // 拖放开始位置
    
    GLubyte m_opacity = 0;
    LayerColor* m_LayerColor;
    bool m_opentouched = false;
    bool m_jumped = false;
    Actor* m_pBoguangActor = nullptr;
    Actor* m_pShipActor = nullptr;
    Sprite* m_pScrewSp = nullptr;
    Sprite* m_pDoorSp = nullptr;
    Sprite* m_pShadowSp = nullptr;
    Sprite* m_pOverSp = nullptr;
};


#endif /* defined(__AbelLLExplorer__Round026_04__) */
