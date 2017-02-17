//
//  RoundUINode.h
//  AbelLLExplorer
//
//  Created by Administrator on 2015/4/16.
//
//

#ifndef __AbelLLExplorer__RoundUINode__
#define __AbelLLExplorer__RoundUINode__

#include "UIBase.h"
#include <vector>

class Actor;
class StageScene;
class StageData;
class SocialShareNode;
class RoundUINode : public UIBase
{
public:
    RoundUINode();
    virtual ~RoundUINode();
    static RoundUINode* create(unsigned int);
public:
    /* 初始化 ui 层 */
    bool initUILayer(const unsigned int);   //两个参数，第一个参数用于区分具体界面返回上层界面的区别,0表示从关卡选择返回主题选择，1表示从回合返回关卡选择
    //更新当前所在回合数显示
    void updateRoundNumber(unsigned int);
    //改变读题actor的动作
    void changeReadActor(unsigned int);
    
    //回合通过播放过关动画
    void playPassActor(unsigned int);
    
    // 添加所有本回合要添加的文字信息
    void updateRoundTextUI(const unsigned int roundIndex);
    // 移除所有本回合文字信息
    void removeAllRoundText();
    
    //开始读题目
    void beganReadTopic();
    
    //题目读完或者中途停止
    void endReadTopic();
    
    //说题目时改变actor动作
    void changeReadActorCall();
    
    Actor* getReadActor(){return m_pReadActor;}
    Label* getTopicLabel(){return m_pTopicLabel;}
    
    //答对题时，停止题目音效
    void stopReadEffect();
    
    bool getIsReadTopic(){return m_isReadTopic;}
    
    //回合结束时停掉节点相关动作
    void stopNodeActions();
    
    //开始回合时，下落阅读actor
    void downReadActor(unsigned int);
    
    SocialShareNode* getSocialShareNode(){return m_pSSNode;}
    
    LayerColor* getLayerColor(){return m_pPassColorLayer;}
    
    //回合结束动画播放完毕隐藏颜色蒙版层和过关回调
    void roundEndCallBack();
    
    //读题完延时回调
    void readTopicOver(float);
    //关闭触摸
    void setCloseTouch();
    //每个主题第一关执行引导
    void addGuideActor(const unsigned int);
    
    //每个主题不需要引导的关卡
    void noneGuideActor(const unsigned int);
    
private:
    ImageView* m_pRoundNumberImageViews[4];    //当前通过回合数
    Actor* m_pReadActor;                      //题目actor
    Actor* m_pPassStarOneActor;               //前三回合通过回合第一个星星actor
    Actor* m_pPassStarTwoActor;               //前三回合通过回合第二个星星actor
    Sprite* m_pStar;                          //前三回合通过时五角星
    Actor* m_pEndPassActor;                   //最后一回合通过actor
    LayerColor* m_pPassColorLayer;            //通过回合时蒙版颜色层
    Scale9Sprite* m_pTopicBaseBoard;          //题目九宫格背景框
    Label* m_pTopicLabel;                     //题目label
    Layer* m_pTouchLayer;                     //触摸响应层
    
    Vec2 m_starsMovePos[4];                   //回合通过星星移动终点坐标组
        
    std::vector<Label*> m_roundLabels;
    
    //当前回合数据
    StageData* m_pStageData = nullptr;
    unsigned int m_curRoundIndex;           //当前回合数
    //是否正在播放题目
    bool m_isReadTopic = false;
    //防止多次点击播放题目按钮
    bool m_touchLimit;
    EventDispatcher* m_pDispatcher;
    EventListenerTouchOneByOne* m_pListenerDrag;
    SocialShareNode* m_pSSNode = nullptr;        // 社交分享
    bool m_isPauseTouch;               //是否暂停了触摸
    bool m_isAllowDownReadActor;          //是否下降了阅读actor
    bool m_isSendCustoms;               //当题目语音没有读完被停止，只派遣一次注册触摸的消息
    bool m_isClosedTouch;                //是否已经关闭触摸，用于提前关闭触摸的情况
    bool m_isTouchPause;                 //判断当前阅读actor的触摸是否被暂停
    bool m_isReadActorDowned = false;            //判断第一回合阅读actor是否掉落完成
};

#endif /* defined(__AbelLLExplorer__RoundUINode__) */
