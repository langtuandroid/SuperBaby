//
//  Round025_04.h
//  AbelLLExplorer
//
//  Created by Administrator on 15/5/6.
//
//

#ifndef __AbelLLExplorer__Round025_04__
#define __AbelLLExplorer__Round025_04__

#include "RoundBase.h"
#include "ui/CocosGUI.h"
USING_NS_CC;
using namespace ui;
class Actor;
class StageScene;
class Round025_04 : public RoundBase
{
public:
    Round025_04();
    virtual ~Round025_04();
    
public:
    bool initWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    static Round025_04* createWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    
public:
    void cleanUp();
    virtual void onEnterRound();
    virtual void celebrate(const float);
    virtual void onPreLoadedRes();
    /* 开启触摸 */
    virtual void openTouch();
    /*  回合结束回调 */
    virtual void endRound();
    
    //海星变大出现
    void scaleStarfishSchedule(float);
    
    //目标海星渐变出现
    void fadeOutStarfishSchedule(float);
    
    //海星依次变大变小
    void scaleChangeStarfishSchedule(float);
    
    //目标海星依次变大变小
    void scaleChangeTargetStarfishSchedule(float);
    
    //分享回调
    void shareCallback();
    
    //回合结束分享调用
    void roundEndShareCallBack(bool isShare);
    
    //移动主角退出
    void moveActor();
    //渐变变黑退出
    void fadeOutLayer(float);
    
    //渐变隐藏海星
    void fadeOutStarfishs();
    
    /* 播放过关引导 */
    void playPassActor();
private:
    SpriteBatchNode* m_pBN = nullptr;

    Sprite* m_pStarfishs[3];            //海星
    ImageView* m_pTargetStarfishs[3];     //目标答案海星
    
    Vec2 m_startfishsPos[3];
    Vec2 m_startfishEndPos[3];
    Vec2 m_targetStartfishPos[3];       
    int m_effectId = 0;                //音效play的id
    int m_InitAniIndex;
    // 拖放答题成功的数量
    unsigned int m_numberOfdragComplete = 0;
    Vec2 m_originalPoint;
    
    Vec2 m_touchBeganPoint;         // 拖放开始位置
    
    std::string m_nameStr[3];
    GLubyte m_opacity = 0;
    LayerColor* m_LayerColor;
    Actor* m_pBoguangActor = nullptr;

};

#endif /* defined(__AbelLLExplorer__Round025_04__) */
