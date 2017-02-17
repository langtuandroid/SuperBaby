//
//  RoundUINode.cpp
//  AbelLLExplorer
//
//  Created by Administrator on 15/4/16.
//
//

#include "RoundUINode.h"
#include "Actor.h"
#include "StageScene.h"
#include "StageData.h"
#include "SocialShareNode.h"
#include "RoundBase.h"
#include "CustomMessage.h"
#include "AppConfig.h"
#include "MyAudioEngine.h"

#define READTOPIC_GAF_CONFIG_FILE               "config/readtopic.json"             //读题actor配置文件
#define READTOPIC_GAF_POS                       Vec2(140,2210)                      //读题actor的位置
#define READTOPIC_GAF_ZORDER                    2                                   //读题actor的z值
#define STANDBYONE                              "standbyone"                        //待机1
#define STANDBYTWO                              "standbytwo"                        //待机2
#define TRIGGER                                 "trigger"                            //触发动作
#define STAR_MOVE_TIME                          0.5f                                //通过回合星星移动时间
#define STAR_ONE_GAF_FILE                       "res/common/xiaoguan-daxingxing/xiaoguan-daxingxing.gaf"        //通过回合时第一个星星
#define STAR_TWO_GAF_FILE                       "res/common/xiaoguan-xiaoxingxing/xiaoguan-xiaoxingxing.gaf"      //通过回合时第二个星星
#define STAR_PNG_FILE                           "res/common/shengli-xingxing.png"                               //star图片
#define STAR_GAF_ONE_ZORDER                     5                                   //第一个星星actor的z值
#define STAR_GAF_TWO_ZORDER                     6                                   //第二个星星actor的z值
#define STAR_PNG_ZORDER                         7                                   //星星图片z值
#define LAYER_COLOR_ZORDER                      4                                   //颜色蒙版层z值
#define STAR_GAF_ONE_POS                        Vec2(50,1600)                       //第一个星星actor的坐标
#define STAR_GAF_TWO_POS                        Vec2(370,1795)                       //第二个星星actor的起始坐标（从左到右）
#define STAR_PNG_POS                            Vec2(1070,830)                       //星星图片坐标
#define ACTOR_ACTION_TAG                        1000                                 //阅读actor睡觉的动作tag
#define LABEL_ACTION_TAG                        1001                                 //label的动作的tag
#define TOUCH_LAYER_ZORDER                      9                                    //触摸层z值
#define SHARE_EFFECT_FILE                       "res/audio/ui/fenxiang.mp3"
#define XINGXING_EFFECT_FILE                    "res/audio/ui/xiaoguanka.mp3"
#define GUIDE_MUSIC_FILE                        "res/audio/PB-TM-TS.mp3"
#define READACTOR_CLICK_MUSIC_FILE              "res/audio/ui/duihuadianji01.mp3"

/*
 *  constructor
 */
RoundUINode::RoundUINode()
:m_isReadTopic(false),
m_touchLimit(false),
m_isPauseTouch(false),
m_isAllowDownReadActor(true),
m_isSendCustoms(false),
m_isClosedTouch(false),
m_isTouchPause(false)
{
    m_starsMovePos[0] = Vec2(784, 1409);
    m_starsMovePos[1] = Vec2(944, 1409);
    m_starsMovePos[2] = Vec2(1104, 1409);
    m_starsMovePos[3] = Vec2(1264, 1409);
    
}

RoundUINode::~RoundUINode()
{
    Director::getInstance()->getEventDispatcher()->removeEventListenersForTarget(m_pTouchLayer);
}

RoundUINode* RoundUINode::create(unsigned int sceneType)
{
    RoundUINode * ret = new RoundUINode();
    if (ret && ret->initUILayer(sceneType))
    {
        ret->autorelease();
    }
    else
    {
        CC_SAFE_DELETE(ret);
    }
    return ret;
}


bool RoundUINode::initUILayer(const unsigned int scenetype)
{
    UIBase::initUILayer(scenetype, false);
    //触摸响应层
    m_pTouchLayer = Layer::create();
    addChild(m_pTouchLayer,4);
    
    //回合通过显示ui
    for(unsigned int i = 0; i < 4; i++)
    {
        m_pRoundNumberImageViews[i] = ImageView::create("UI-xingxing02.png",cocos2d::ui::Widget::TextureResType::PLIST);
        m_pRoundNumberImageViews[i]->setPosition(m_starsMovePos[i]);
        addChild(m_pRoundNumberImageViews[i]);
    }
    
    //题目actor
    m_pReadActor = Actor::createWithGafConfigFile(READTOPIC_GAF_CONFIG_FILE);
    m_pReadActor->setPosition(READTOPIC_GAF_POS);
    m_pReadActor->setSize(Size(180, 180));
    m_pReadActor->setPosAtNode(Vec2(-160, 390));
    m_pReadActor->setAnchorPoint(Vec2(0.5f, 0.5f));
    m_pTouchLayer->addChild(m_pReadActor, READTOPIC_GAF_ZORDER);
    m_pReadActor->playWithSequenceAliasName(STANDBYONE,true);
    
    //通过回合时actor
    m_pPassStarOneActor = Actor::createWithGafFile(STAR_ONE_GAF_FILE,false);   //第一个星星actor
    m_pPassStarOneActor->setPosition(STAR_GAF_ONE_POS);
    m_pTouchLayer->addChild(m_pPassStarOneActor, STAR_GAF_ONE_ZORDER);
    m_pPassStarOneActor->setVisible(false);
    m_pPassStarOneActor->stop();
    
    m_pPassStarTwoActor = Actor::createWithGafFile(STAR_TWO_GAF_FILE,false);   //第二个星星actor
    m_pPassStarTwoActor->setPosition(STAR_GAF_TWO_POS);
    m_pTouchLayer->addChild(m_pPassStarTwoActor, STAR_GAF_TWO_ZORDER);
    m_pPassStarTwoActor->setVisible(false);
    m_pPassStarTwoActor->stop();
    
    m_pStar = Sprite::create(STAR_PNG_FILE);                            //星星图片
    m_pStar->setPosition(STAR_PNG_POS);
    m_pTouchLayer->addChild(m_pStar, STAR_PNG_ZORDER);
    m_pStar->setVisible(false);
    
    //颜色蒙版层
    auto size = Director::getInstance()->getVisibleSize();
    m_pPassColorLayer = LayerColor::create(Color4B(0, 0, 0, 150), size.width, size.height);
    m_pTouchLayer->addChild(m_pPassColorLayer,LAYER_COLOR_ZORDER);
    m_pPassColorLayer->setVisible(false);
    
    //题目对话框
    m_pTopicBaseBoard = Scale9Sprite::createWithSpriteFrameName("UI-duihuakuang.png", Rect(521, 111, 1, 1));
    m_pTopicBaseBoard->setPosition(Vec2(240, 1220));
    m_pTopicBaseBoard->setAnchorPoint(Vec2(0.0f,0.5f));
    m_pTopicBaseBoard->setInsetTop(0.5f);
    m_pTouchLayer->addChild(m_pTopicBaseBoard, 1);
    m_pTopicBaseBoard->setVisible(false);
    //    m_pTopicBaseBoard->setOpacity(105);
    
    //分享界面
    m_pSSNode = SocialShareNode::create();    //分享
    m_pSSNode->setPosition(Vec2(0,0));
    addChild(m_pSSNode, 4);
    m_pSSNode->setVisible(false);
    m_pSSNode->attachUINode(this);
    
    //注册触摸监听
    // 事件派发
    m_pDispatcher = Director::getInstance()->getEventDispatcher();
    m_pListenerDrag = EventListenerTouchOneByOne::create();
    //    listenerDrag->setSwallowTouches(true);
    m_pListenerDrag->onTouchBegan = [this](Touch* touch, Event* event) -> bool
    {
        auto target = static_cast<Node*>(event->getCurrentTarget());
        auto touchPoint = target->convertTouchToNodeSpace(touch);
        Rect rect = Rect(0.0f, 0.0f, target->getContentSize().width, target->getContentSize().height);
        if (m_touchLimit) {    //限制生效
            return false;
        }else
        {
            if (m_isReadActorDowned) {
                if (rect.containsPoint(touchPoint))
                {
                    m_pReadActor->setScale(1.2f);
                }
                return true;
            }
        }
        return false;
    };
    m_pListenerDrag->onTouchEnded = [this](Touch* touch, Event* event)
    {
        m_pReadActor->setScale(1.0f);
        auto target = static_cast<Node*>(event->getCurrentTarget());
        auto touchPoint = target->convertTouchToNodeSpace(touch);
        Rect rect = Rect(0.0f, 0.0f, target->getContentSize().width, target->getContentSize().height);
        if (rect.containsPoint(touchPoint))
        {
            MyAudioEngine::getInstance()->playAudio(READACTOR_CLICK_MUSIC_FILE);
            if (m_pReadActor->getActionByTag(ACTOR_ACTION_TAG)) {
                m_pReadActor->stopAllActionsByTag(ACTOR_ACTION_TAG);              //移除睡觉的延时动作
            }
            if (m_pTopicLabel != nullptr) {
                if (m_pTopicLabel->getActionByTag(LABEL_ACTION_TAG)) {
                    m_pTopicLabel->stopAllActionsByTag(LABEL_ACTION_TAG);              //移除label的动作
                }
            }
            if (m_isReadTopic) {
                unschedule(CC_SCHEDULE_SELECTOR(RoundUINode::readTopicOver));
                stopReadEffect();
                endReadTopic();                                                     //停止读题目改变动作
                if (!m_isSendCustoms) {
                    auto dispatcher = Director::getInstance()->getEventDispatcher();
                    dispatcher->dispatchCustomEvent("openTouch");
                    m_isSendCustoms = true;
                }
            }else
            {
                beganReadTopic();        //开始读题
            }
        }
    };
    m_pDispatcher->addEventListenerWithSceneGraphPriority(m_pListenerDrag, m_pReadActor);
    
    return true;
}

/*
 *  添加所有本回合要添加的文字信息
 */
void RoundUINode::updateRoundTextUI(const unsigned int roundIndex)
{
    removeAllRoundText();
    
    if (m_isPauseTouch) {
        m_pDispatcher->resumeEventListenersForTarget(m_pReadActor);  //恢复触摸
        m_pReturnBtn->setEnabled(true);
        m_pMusicSwitchBtn->setEnabled(true);
        m_pDispatcher->resumeEventListenersForTarget(m_pBagActor);
        m_isPauseTouch = false;             //恢复触摸
        m_isClosedTouch = false;            //打开了触摸
    }
    
    //获得当前回合数
    m_curRoundIndex = roundIndex;
    
    // 获取关联的关卡数据
    m_pStageData = m_pStageScene->getStageData();
    
    // 获取本回合要显示的所有文字
    std::vector<RoundTextUnit> roundTextUnits;
    m_pStageData->getRoundText(roundIndex, roundTextUnits);
    
    if (roundTextUnits.empty()) {
        m_pDispatcher->pauseEventListenersForTarget(m_pReadActor);
        m_isTouchPause = true;
        return;
    }else
    {
        for (auto &it : roundTextUnits)
        {
            TTFConfig ttfConfig;
            ttfConfig.fontFilePath = it.fontFile;
            ttfConfig.fontSize = it.fontSize;
            ttfConfig.distanceFieldEnabled = false;
            ttfConfig.outlineSize = 0;
            ttfConfig.glyphs = GlyphCollection::DYNAMIC;
            ttfConfig.customGlyphs = nullptr;
            
            // 使用TTFConfig配置，来创建TTF
            m_pTopicLabel = Label::createWithTTF(ttfConfig,
                                                 it.text,
                                                 TextHAlignment::LEFT,
                                                 it.maxLineWidth);
            m_pTopicLabel->setPosition(Vec2(it.posX, it.posY));
            m_roundLabels.push_back(m_pTopicLabel);
            addChild(m_pTopicLabel,5);
            m_pTopicLabel->setOpacity(0);
            m_pTopicLabel->setColor(Color3B(255, 106, 124));
        } /*for*/
        
        m_pTopicBaseBoard->setVisible(true);
        
        m_pTopicBaseBoard->setContentSize(Size(roundTextUnits[0].maxLineWidth+110,222));
        
        if (m_isTouchPause) {           //当前一回合是收npc被暂停触摸，在此恢复触摸
            m_pDispatcher->resumeEventListenersForTarget(m_pReadActor);
            m_isTouchPause = false;
        }
        
        beganReadTopic();   //开始读题目
    }
}

//开始读题目
void RoundUINode::beganReadTopic()
{
    m_pTopicBaseBoard->setScale(0.0f);
    m_touchLimit = true;    //启动限制
    m_pTopicBaseBoard->runAction(Sequence::create(EaseBackOut::create(ScaleTo::create(0.2f, 1.0f)),CallFunc::create([this]{
        m_pTopicLabel->runAction(Sequence::create(FadeIn::create(0.2f),CallFunc::create([this]{
            m_touchLimit = false;     //结束限制
        }), NULL));
        m_pReadActor->playWithSequenceAliasName(STANDBYONE,true);
        // 题目音效
        m_topicAudioId = MyAudioEngine::getInstance()->playAudio(m_pStageData->getQaudioFile(m_curRoundIndex).c_str());
        m_isReadTopic = true;    //当前正在播放音效
        m_isBagEnabled = false;   //限制背包可触摸
        changeReadActorCall();       //改变阅读actor
    }), NULL));
}

//题目读完或者中途停止
void RoundUINode::endReadTopic()
{
    m_isReadTopic = false;      //当前音效已经播完
    m_touchLimit = true;        //启动限制
    m_isBagEnabled = true;     //恢复背包可触摸
    m_pReadActor->playWithSequenceAliasName(STANDBYONE,true);
    MyAudioEngine::getInstance()->unloadAudio(m_pStageData->getQaudioFile(m_curRoundIndex)); //清除声音缓存
    unschedule(CC_SCHEDULE_SELECTOR(RoundUINode::readTopicOver));
    auto seq = Sequence::create(FadeOut::create(0.1f),CallFunc::create([this]{
        m_pTopicBaseBoard->runAction(Sequence::create(ScaleTo::create(0.2f, 0.0f),CallFunc::create([this]{
            m_touchLimit = false;        //结束限制
        }), NULL));
        auto seq = Sequence::create(DelayTime::create(30),CallFunc::create([this]{
            m_pReadActor->playWithSequenceAliasName(STANDBYTWO,true);
        }), NULL);
        seq->setTag(ACTOR_ACTION_TAG);
        m_pReadActor->runAction(seq);
    }), NULL);
    seq->setTag(LABEL_ACTION_TAG);
    m_pTopicLabel->runAction(seq);
}

//改变阅读actor
void RoundUINode::changeReadActorCall()
{
    m_pReadActor->playWithSequenceAliasName(TRIGGER,true);
    this->scheduleOnce(CC_SCHEDULE_SELECTOR(RoundUINode::readTopicOver), m_pStageData->getQaudioTime(m_curRoundIndex));
}

//读题完延时回调
void RoundUINode::readTopicOver(float)
{
    endReadTopic();         //题目读完
    if (!m_isSendCustoms) {
        auto dispatcher = Director::getInstance()->getEventDispatcher();
        dispatcher->dispatchCustomEvent("openTouch");
        m_isSendCustoms = true;
    }
}

//答对题时，停止题目音效
void RoundUINode::stopReadEffect()
{
    MyAudioEngine::getInstance()->stopAudio(m_topicAudioId);
    m_topicAudioId = 0;
}

/*
 *  移除所有本回合文字信息
 */
void RoundUINode::removeAllRoundText()
{
    for (auto &it : m_roundLabels) {
        it->removeFromParent();
        it = nullptr;
    }
    m_roundLabels.clear();
}

//更新当前所在回合数显示
void RoundUINode::updateRoundNumber(unsigned int number)
{
    m_pRoundNumberImageViews[number] = ImageView::create("UI-xingxing01.png",cocos2d::ui::Widget::TextureResType::PLIST);
}

//改变读题actor的动作
void RoundUINode::changeReadActor(unsigned int index)
{
    if (index == 1) {
        m_pReadActor->playWithSequenceAliasName(STANDBYTWO,true);
    }else if (index == 2)
    {
        m_pReadActor->playWithSequenceAliasName(TRIGGER);
        m_pReadActor->setAnimationFinishedPlayDelegate([this]
                                                       {
                                                           m_pReadActor->playWithSequenceAliasName(STANDBYONE,true);
                                                       });
    }
}

//关闭触摸
void RoundUINode::setCloseTouch()
{
    m_pDispatcher->pauseEventListenersForTarget(m_pReadActor);   //暂停触摸
    m_pReturnBtn->setEnabled(false);
    m_pMusicSwitchBtn->setEnabled(false);
    m_isClosedTouch = true;        //已经关闭了触摸
}

//回合通过播放过关动画
void RoundUINode::playPassActor(unsigned int index)
{
    if (!m_isClosedTouch) {
        setCloseTouch();
    }else
    {
        m_pDispatcher->pauseEventListenersForTarget(m_pReadActor);
    }
    MyAudioEngine::getInstance()->playAudio(XINGXING_EFFECT_FILE);   //播放音效
    
    m_isPauseTouch = true;              //暂停了触摸
    m_isBagEnabled = false;             //回合结束，限制背包触摸
    m_pPassColorLayer->setVisible(true);
    m_pPassStarOneActor->start();
    m_pPassStarOneActor->setVisible(true);
    m_pPassStarOneActor->setAnimationFinishedPlayDelegate([this,index]
                                                          {
                                                              m_pPassStarOneActor->stop();
                                                              m_pPassStarOneActor->setVisible(false);
                                                              m_pStar->setVisible(true);          //显示星星
                                                              m_pStar->runAction(Sequence::create(Spawn::create(ScaleTo::create(STAR_MOVE_TIME, 0.3f),EaseExponentialOut::create(MoveTo::create(STAR_MOVE_TIME, m_starsMovePos[index])), NULL),CallFunc::create([this,index]{
                                                                  m_pRoundNumberImageViews[index]->loadTexture("UI-xingxing01.png",cocos2d::ui::Widget::TextureResType::PLIST);
                                                                  m_pPassColorLayer->setVisible(false);     //隐藏颜色蒙版层
                                                                  m_pStar->setVisible(false);
                                                              }),ScaleTo::create(STAR_MOVE_TIME-0.4f, 0.26f),CallFunc::create([this,index]{
                                                                  m_pPassStarTwoActor->setPosition(Vec2(STAR_GAF_TWO_POS.x+160*index, STAR_GAF_TWO_POS.y));
                                                                  m_pPassStarTwoActor->setVisible(true);
                                                                  m_pPassStarTwoActor->start();
                                                                  m_pPassStarTwoActor->setAnimationFinishedPlayDelegate([this,index]
                                                                                                                        {
                                                                                                                            m_pPassStarTwoActor->stop();
                                                                                                                            m_pPassStarTwoActor->setVisible(false);
                                                                                                                            m_pStar->setPosition(STAR_PNG_POS);
                                                                                                                            
                                                                                                                            if (m_pStageScene->isPopShareNode(m_curRoundIndex)) {
                                                                                                                                auto dispatcher = Director::getInstance()->getEventDispatcher();
                                                                                                                                dispatcher->dispatchCustomEvent(SHARE_CALLBACK);
                                                                                                                                MyAudioEngine::getInstance()->playAudio(SHARE_EFFECT_FILE); //分享音效
                                                                                                                                m_pSSNode->setVisible(true);
                                                                                                                                m_pSSNode->setStageIndex(m_pStageScene->getStageData()->getStageIndex());
                                                                                                                                m_pSSNode->setScaleShareNode();
                                                                                                                                m_pSSNode->getShareActor()->start();
                                                                                                                                if (m_pStageScene->getStageData()->isNpcRound(m_curRoundIndex)) {
                                                                                                                                    dispatcher->addCustomEventListener(ROUND_END_CALLBACK, [this] (EventCustom *) {
                                                                                                                                        Director::getInstance()->getEventDispatcher()->removeCustomEventListeners(ROUND_END_CALLBACK);
                                                                                                                                        roundEndCallBack();
                                                                                                                                    });
                                                                                                                                }
                                                                                                                            } else
                                                                                                                            {
                                                                                                                                if (m_pStageScene->getStageData()->isNpcRound(m_curRoundIndex)) {
                                                                                                                                    auto dispatcher = Director::getInstance()->getEventDispatcher();
                                                                                                                                    dispatcher->addCustomEventListener(ROUND_END_CALLBACK, [this] (EventCustom *) {
                                                                                                                                        Director::getInstance()->getEventDispatcher()->removeCustomEventListeners(ROUND_END_CALLBACK);
                                                                                                                                        roundEndCallBack();
                                                                                                                                    });
                                                                                                                                    dispatcher->dispatchCustomEvent("collectNpc");     //收集npc
                                                                                                                                }else
                                                                                                                                {
                                                                                                                                    roundEndCallBack();   //回合结束动画播放完毕隐藏颜色蒙版层和过关回调
                                                                                                                                }
                                                                                                                            }
                                                                                                                        });
                                                              }), NULL));
                                                          });
}

//回合结束动画播放完毕隐藏颜色蒙版层和过关回调
void RoundUINode::roundEndCallBack()
{
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->dispatchCustomEvent("endRound");
    
    stopNodeActions();                        //停止相关action
    m_isSendCustoms = false;                 //新的回合，重新置为false，新回合可以重新派遣
}

//回合结束时停掉节点相关动作
void RoundUINode::stopNodeActions()
{
    if (m_pReadActor->getActionByTag(ACTOR_ACTION_TAG)) {  //停掉ui界面相关action
        m_pReadActor->stopAllActionsByTag(ACTOR_ACTION_TAG);
    }
    
    if (m_pTopicLabel != nullptr) {
        if (m_pTopicLabel->getActionByTag(LABEL_ACTION_TAG)) {
            m_pTopicLabel->stopAllActionsByTag(LABEL_ACTION_TAG);
        }
    }
}

//开始回合时，下落阅读actor
void RoundUINode::downReadActor(unsigned int roundIndex)
{
    
    if (m_isAllowDownReadActor) {          //每一关掉落一次阅读actor
        if (m_pStageScene->getStageData()->getStageIndex()%12 == 0) {
            if (m_pStageScene->getStageData()->getStageIndex()/12 == 0) {
                if (!UserDefault::getInstance()->getBoolForKey("readLeadOne", false)) {
                    UserDefault::getInstance()->setBoolForKey("readLeadOne", true);
                    addGuideActor(roundIndex);
                }else
                {
                    noneGuideActor(roundIndex);
                }
            }else if (m_pStageScene->getStageData()->getStageIndex()/12 == 1)
            {
                if (!UserDefault::getInstance()->getBoolForKey("readLeadTwo", false)) {
                    UserDefault::getInstance()->setBoolForKey("readLeadTwo", true);
                    addGuideActor(roundIndex);
                }else
                {
                    noneGuideActor(roundIndex);
                }
            }else if (m_pStageScene->getStageData()->getStageIndex()/12 == 2)
            {
                if (!UserDefault::getInstance()->getBoolForKey("readLeadThree", false)) {
                    UserDefault::getInstance()->setBoolForKey("readLeadThree", true);
                    addGuideActor(roundIndex);
                }else
                {
                    noneGuideActor(roundIndex);
                }
            }else if (m_pStageScene->getStageData()->getStageIndex()/12 == 3)
            {
                if (!UserDefault::getInstance()->getBoolForKey("readLeadFour", false)) {
                    UserDefault::getInstance()->setBoolForKey("readLeadFour", true);
                    addGuideActor(roundIndex);
                }else
                {
                    noneGuideActor(roundIndex);
                }
            }
        }else
        {
            noneGuideActor(roundIndex);
        }
        
    }else
    {
        // 文字显示
        updateRoundTextUI(roundIndex);
    }
}

//每个主题第一关执行引导
void RoundUINode::addGuideActor(const unsigned int roundIndex)
{
    auto size = Director::getInstance()->getVisibleSize();
    auto dw = AppConfig::getInstance()->getDesignResolutionWidth();
    auto colorLayer = LayerColor::create(Color4B(0, 0, 0, 150), size.width, size.height);
    this->addChild(colorLayer,1);
    Director::getInstance()->getEventDispatcher()->pauseEventListenersForTarget(m_pReadActor);
    Director::getInstance()->getEventDispatcher()->pauseEventListenersForTarget(m_pBagActor);
    m_pReturnBtn->setEnabled(false);
    m_pMusicSwitchBtn->setEnabled(false);
    m_pReadActor->setPosition(Vec2(dw/2, 2210));
    m_pReadActor->runAction(Sequence::create(Spawn::create(EaseBackOut::create(MoveTo::create(1.0f, Vec2(dw/2, size.height/2))),ScaleTo::create(1.0f, 1.4f), NULL),CallFunc::create([this,roundIndex,colorLayer]{
        //改变阅读状态
        m_pReadActor->playWithSequenceAliasName(TRIGGER,true);
        //播放声音
        MyAudioEngine::getInstance()->playAudio(GUIDE_MUSIC_FILE);
        scheduleOnce([this,roundIndex,colorLayer](float){
            m_pReadActor->playWithSequenceAliasName(STANDBYONE,true);
            m_pReadActor->runAction(Sequence::create(Spawn::create(MoveTo::create(1.0f, Vec2(140, 1210)),ScaleTo::create(1.0f, 1.0f), NULL),CallFunc::create([this,roundIndex,colorLayer]{
                scheduleOnce([this](float){
                    Director::getInstance()->getEventDispatcher()->resumeEventListenersForTarget(m_pReadActor);
                },8.0f,"delaytouchUI");
                scheduleOnce([this](float){
                    Director::getInstance()->getEventDispatcher()->resumeEventListenersForTarget(m_pBagActor);
                },8.0f,"delaytouchUIBase");
                m_pReturnBtn->setEnabled(true);
                m_pMusicSwitchBtn->setEnabled(true);
                colorLayer->removeFromParent();
                m_isAllowDownReadActor = false;     //每一关掉落一次后不再允许掉落
                m_isReadActorDowned = true;
                // 文字显示
                updateRoundTextUI(roundIndex);
                openBagTouch();        //开启背包触摸
            }), NULL));
        }, 4.0f, "playMusic");
    }), NULL));
}

//每个主题不需要引导的关卡
void RoundUINode::noneGuideActor(const unsigned int roundIndex)
{
    m_pReadActor->runAction(Sequence::create(EaseBackOut::create(MoveBy::create(1.0f, Vec2(0, -1000))),CallFunc::create([this,roundIndex]{
        m_isAllowDownReadActor = false;     //每一关掉落一次后不再允许掉落
        m_isReadActorDowned = true;
        // 文字显示
        updateRoundTextUI(roundIndex);
        openBagTouch();        //开启背包触摸
    }), NULL));
}

