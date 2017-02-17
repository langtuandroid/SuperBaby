//
//  Round007_04.h
//  AbelLLExplorer
//
//  Created by Administrator on 15/4/3.
//
//

#ifndef __AbelLLExplorer__Round007_04__
#define __AbelLLExplorer__Round007_04__

#include "RoundBase.h"
USING_NS_CC;

class Actor;
class SocialShareNode;
class StageScene;
class Round007_04 : public RoundBase
{
public:
    Round007_04();
    virtual ~Round007_04();
    
public:
    bool initWithRoundIndex(const int,StageScene*, const Vec2& pos = Vec2::ZERO);
    static Round007_04* createWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    
public:
    virtual void celebrate(const float);
    virtual void onPreLoadedRes();
    virtual void openTouch();
    /*  回合结束回调 */
    virtual void endRound();

    // 回合开场动画，符号上升动画调度
    void BeganMarksUpSchedule(float);
    // 回合开场动画，藤条下降动画调度
    void BeganVinesDownSchedule(float);
    
    //藤条下降结束完成
    void vineDownComplete();
    
    // 回合结束动画，藤条上升动画调度
    void EndVineUpSchedule(float);
    
    //移动主角退出
    void moveActor();

    //回合结束分享调用
    void roundEndShareCallBack(bool isShare);
    
    //分享回调
    void shareCallback();
    
    //渐变变黑退出
    void fadeOutLayer(float);
    
private:
    void cleanUp();
    virtual void onEnterRound();
    
private:
    SpriteBatchNode* m_pBN;     //物品BN
    Sprite* m_pAnswerMarks[4];  //答案符号组
    Sprite* m_pMarks[4];        //目标符号组
    Sprite* m_pVine[4];         //藤组
    Sprite* m_pFruits[4];       //水果组
    Sprite* m_pTree;            //树
    
    Vec2 m_answerMarksPos[4];   //答案符号坐标组
    Vec2 m_marksPos[4];         //目标符号坐标组
    Vec2 m_vinesPos[4];         //藤坐标组
    Vec2 m_fruitsPos[4];        //水果坐标组
    
    SocialShareNode* m_pSSNode;        // 社交分享

    Vec2 m_originalPoint;
    Vec2 m_touchBeganPoint;
    int m_correctNumber;
    unsigned int m_upIndex;         //当前上升的index
    unsigned int m_downIndex;       //当前下降的index
    int m_effectId;
    Vec2 m_originalPoints[4];
    GLubyte m_opacity = 0;
    bool m_opentouched = false;
    bool m_jumped = false;
};

#endif /* defined(__AbelLLExplorer__Round007_04__) */
