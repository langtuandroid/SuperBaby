//
//  Actor.h
//  AbelLLExplorer
//
//  Created by adinnet on 2015/3/6.
//
//  演员角色类
//  封装了 gaf 动画文件的加载、播放等功能
//

#ifndef __AbelLLExplorer__Actor__
#define __AbelLLExplorer__Actor__

#include "cocos2d.h"
#include <map>

USING_NS_CC;

typedef std::function<void ()> CALLBACKFUNCTION;

namespace gaf
{
    class GAFAsset;
    class GAFObject;
}

class Actor : public Node
{
public:
    Actor();
    virtual ~Actor();

public:
    /* 根据 gaf 文件生成角色 */
    static Actor* createWithGafFile(const std::string& gafFile, const bool looped = true,
                                    const float scaleFactor = 1.0f, const Vec2& posAtNode = Vec2::ZERO);
    /* 根据 gaf 配置文件生成角色 */
    static Actor* createWithGafConfigFile(const std::string&);
    /* 根据其它 Actor 对象生成角色 */
    static Actor* createWithActor(Actor*, const bool looped = true);

public:
    const Size getActorContentSize() const;
    
public:
    /* 根据 gaf 文件初始化 */
    bool initWithGafFile(const std::string&, const bool, const float, const Vec2&);
    /* 根据 gaf 配置文件初始化 */
    virtual bool initWithGafConfigFile(const std::string&);
    /* 根据其他 actor 对象初始化 */
    bool initWithActor(Actor*,  const bool looped = true);
    
public:
    /* 根据动作序列别名播放相应的序列动画 */
    void playWithSequenceAliasName(const std::string&, const bool looped = false);
    
    /* 根据动作序列组别名按顺序播放相应的序列组动画,此情况，每个动作都是不循环的（只播一次） */
    void playWithSequenceAliasName(const std::vector<std::string>& strsSequence, const bool lastLooped = false, const unsigned int loopNum = 1, const bool isRepeatForever = false, CALLBACKFUNCTION callBackFunction = nullptr);
    
    /* 是否主角 */
    virtual bool isLeadingActor() const {return false;}
    
    /* 是否可收集 NPC */
    virtual bool isNpcActor() const {return false;}
    
    /* 获取 GAFAsset 对象 */
    inline gaf::GAFAsset* getGAFAsset() const {return m_pGafAsset;}
    /* 获取和设置 gaf 对象在结点中的位置 */
    inline Vec2 getPosAtNode() const {return m_posAtNode;}
    void setPosAtNode(const Vec2&);
    
    /* 获取缩放因子 */
    inline float getScaleFactor() const {return m_scaleFactor;}
    /* 设置缩放因子 */
    void setScaleFactor(const float);
    
    /* 获取动作别名列表 */
    inline const std::map<std::string, std::string>& getAliasForSequence() const {return m_aliasForSequence;}

    /* 是否动画播放中 */
    bool IsAnimationRunning();
    
    /* 设置动画结束回调通知函数 */
    void setAnimationFinishedPlayDelegate(const std::function<void()>&);
    
    /* 获取 gafObj 对象 */
    inline gaf::GAFObject* getGafObj() const {return m_pGafObj;}
    
    /* 是否循环播放 gaf 动画 */
    inline bool isLooped () const {return m_looped;}
    
    /* 获取 gaf 文件 */
    std::string getGafFile() const {return m_gafFile;}
    
    /**/
    void setGafObjVisible(const bool);
    
    /* 获取 key 值 */
    std::string getKey() const {return m_key;}
    
    /* 获取和设置 gaf 对象显示区域大小 */
    inline Size getSize() const {return m_Size;}
    void setSize(const cocos2d::Size&);
    /* 获得包围矩形 */
    inline Rect getBoundingRect()
    {return Rect(getPosition().x , getPosition().y, m_Size.width, m_Size.height); }

public:
    void start();
    void stop();
    void onAnimationFinished(gaf::GAFObject*);
    int getGafObjZorder();
    
    /**/
    inline void setAutoReleaseAsset(const bool autoreleaseAsset) {m_autoreleaseAsset = autoreleaseAsset;}
    
protected:
    std::string m_key = "";
    
    gaf::GAFAsset *m_pGafAsset = nullptr;
    gaf::GAFObject *m_pGafObj = nullptr;
    
    /* gaf 文件 */
    std::string m_gafFile = "";
    /* 是否循环播放 */
    bool m_looped = false;
    
    /* gaf 动画对象在结点中的位置 */
    Vec2 m_posAtNode = Vec2::ZERO;
    /* 缩放因子 */
    float m_scaleFactor = 1.0f;
    
    // 存放动作别名和对应（预定义）动作序列名称的列表
    std::map<std::string, std::string> m_aliasForSequence;
    
    std::function<void ()> m_AnimationFinishedPlayCB = nullptr;

    // gaf 对象区域大小
    cocos2d::Size m_Size = cocos2d::Size(0.0f, 0.0f);
    
    /* 在屏幕上显示区域矩形框、调试用 */
    cocos2d::DrawNode* m_pDebugDrawNode = nullptr;
    
private:
    void drawNode();
    
    void _generateGafObject();

private:
    int m_gafObjZorder = 0;
    unsigned int m_curIndex = 0;
    unsigned int m_curRepeatNum = 0;
    bool m_autoreleaseAsset = false;
};

#endif /* defined(__AbelLLExplorer__Actor__) */
