//
//  Round011_03.cpp
//  AbelLLExplorer
//
//  Created by Administrator on 2015/4/9.
//
//

#include "Round011_03.h"
#include "StageScene.h"
#include "StageData.h"
#include "Actor.h"
#include "MyAudioEngine.h"
#include "RoundUINode.h"

#define CELEBRATE_DELAY                  5.0f                                    // 庆祝延时
#define BN_PLIST_FILE                  "res/stage011/SL-GK11-IT.plist"           //BN的plist文件
#define BN_PNG_FILE                    "res/stage011/SL-GK11-IT.png"             //BN的png文件
#define BN_ZORDER                      2                                         //BN的z值
#define STANDBY                          "standby"                               // 待机动画
#define TRIGGER                          "trigger"                               // 触发动画
#define DOCTOR_GAF_CONFIG_FILE          "config/stage011/doctor.json"            //医生gaf配置文件
#define FIREMAN_GAF_CONFIG_FILE         "config/stage011/fireman.json"           //消防员gaf配置文件
#define POLICEMAN_GAF_CONFIG_FILE       "config/stage011/policeman.json"          //警察gaf配置文件
#define ACTOR_ZORDER                    1                                          //actor的z值
#define TOOLS_ZORDER                    1                                          //工具z值
#define TOOLS_SCALE_TIME                0.5f                                       //工具放大时间
#define MOV_TIME                       0.2f                                    //答错回原位置移动时间
#define TOOL_FADEOUT_TIME             0.5f                                    //工具到达目标区域渐变消失时间
#define ACTORS_CHANGE_TIME            0.5f                                    //离开回合时actor改变时间
#define ACTORS_CHANGE_POS             Vec2(280,70)                          //离开回合时actor相对位移

/*
 *  constructor
 */
Round011_03::Round011_03()
:m_InitAniIndex(0),
m_correctNumber(0)
{
    //设置actor坐标
    m_officeworkersPos[0] = Vec2(202, 1159);
    m_officeworkersPos[1] = Vec2(269, 1159);
    m_officeworkersPos[2] = Vec2(357, 1159);
    
    //设置工具坐标
    m_toolsPos[0] = Vec2(980, 899);
    m_toolsPos[1] = Vec2(1320, 899);
    m_toolsPos[2] = Vec2(1754, 899);
    
    //设置触摸响应区
    m_rect[0] = Rect(780, 200, 250, 350);
    m_rect[1] = Rect(1180, 200, 250, 350);
    m_rect[2] = Rect(1580, 200, 250, 350);
    
    //设置触摸响应区的中点
    m_centerRectsPos[0] = Vec2(905, 375);
    m_centerRectsPos[1] = Vec2(1305, 375);
    m_centerRectsPos[2] = Vec2(1705, 375);
}

/*
 *  destructor
 */
Round011_03::~Round011_03() {
    cleanUp();
}

//
Round011_03* Round011_03::createWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    auto pRound = new Round011_03();
    if (pRound && pRound->initWithRoundIndex(roundIndex,pStageScene,pos))
    {
        pRound->autorelease();
    } else {
        CC_SAFE_DELETE(pRound);
    }
    return pRound;
}

/*
 *  初始化
 */
bool Round011_03::initWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    m_pStageScene = pStageScene;
    m_roundIndex = roundIndex;
    
    setAnchorPoint(Vec2::ZERO);
    setPosition(pos);
    
    appendAsyncLoadGaf("res/stage011/SL-GK11-3-MS02-D+C/SL-GK11-3-MS02-D+C.gaf");
    appendAsyncLoadGaf("res/stage011/SL-GK11-3-MS01-D+C/SL-GK11-3-MS01-D+C.gaf");
    appendAsyncLoadGaf("res/stage011/SL-GK11-3-MS03-D+C/SL-GK11-3-MS03-D+C.gaf");
    preLoadRes();
    return true;
}
/*
 *  清理
 */
void Round011_03::cleanUp()
{
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->removeCustomEventListeners("openTouch");
    dispatcher->removeCustomEventListeners("endRound");
}

int Round011_03::randomIndex(int min,int max)
{
    srand(unsigned(time(0)));
    return random(min, max);
}

bool Round011_03::numberIsRepeat(std::vector<int>& indexs,int number)
{
    while (true)
    {
        int cursize = 0;
        if (indexs.size() != 0) {
            for (auto &it : indexs)
            {
                cursize++;
                if (it == number) {
                    return false;    //表示重复
                }else
                {
                    if (cursize == indexs.size()) {
                        return true;  //表示不重复
                    }
                }
            }
        }else
        {
            return true;          //表示为随机的第一个数，没有重复
        }
    }
}

int Round011_03::getRandomNumber(std::vector<int>& indexs,int min, int max)
{
    while (true) {
        auto number = randomIndex(min,max);  //获得随机数
        auto isrepeat = numberIsRepeat(indexs,number);  //判断是否重复
        if (isrepeat) {
            indexs.push_back(number);
            return number;
        }
    }
}

//
void Round011_03::onPreLoadedRes()
{
    ///////
    RoundBase::onPreLoadedRes();
    
    //加载actor
    for (unsigned int i = 0; i < 3; i++) {
        if (i == 0) {
            m_pOfficeworkers[i] = Actor::createWithGafConfigFile(DOCTOR_GAF_CONFIG_FILE);
        }else if (i == 1)
        {
            m_pOfficeworkers[i] = Actor::createWithGafConfigFile(FIREMAN_GAF_CONFIG_FILE);
        }else
        {
            m_pOfficeworkers[i] = Actor::createWithGafConfigFile(POLICEMAN_GAF_CONFIG_FILE);
        }
        m_pOfficeworkers[i]->setPosition(m_officeworkersPos[i]);
        m_pOfficeworkers[i]->playWithSequenceAliasName(STANDBY,true);
        addChild(m_pOfficeworkers[i], ACTOR_ZORDER);
    }
    
    // 纹理加载
    m_pBN = SpriteBatchNode::create(BN_PNG_FILE);
    addChild(m_pBN, BN_ZORDER);
    
    //加载工具
    char gjBuffer[64];
    for (unsigned long i = 0; i < 3; i++) {
        sprintf(gjBuffer, "SL-GK11-3-DA%02ld.png",i+1);
        m_pTools[i] = Sprite::createWithSpriteFrameName(gjBuffer);
        auto index = getRandomNumber(m_pCreatedIndexs, 0, 2);
        m_pTools[i]->setPosition(m_toolsPos[index]);
        m_originalPoints[i] = m_toolsPos[index];
        m_pTools[i]->setUserData((void*)i);
        m_pBN->addChild(m_pTools[i], TOOLS_ZORDER);
        m_pTools[i]->setScale(0.0f);
    } /*for*/
    m_pCreatedIndexs.clear();   //清掉
}

//开始回合
void Round011_03::onEnterRound()
{
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->addCustomEventListener("openTouch", [this] (EventCustom *) {
        auto dispatcher = Director::getInstance()->getEventDispatcher();
        dispatcher->removeCustomEventListeners("openTouch");
        m_opentouched = true;
        if (m_jumped) {
            openTouch();
        }
    });
    dispatcher->addCustomEventListener("endRound", [this] (EventCustom *) {
        auto dispatcher = Director::getInstance()->getEventDispatcher();
        dispatcher->removeCustomEventListeners("endRound");
        endRound();
    });
    this->schedule(CC_SCHEDULE_SELECTOR(Round011_03::showToolsSchedule), TOOLS_SCALE_TIME, -1, 0);
}

//回合开始，依次显示工具
void Round011_03::showToolsSchedule(float)
{
    if (m_InitAniIndex >= 3) {
        unschedule(CC_SCHEDULE_SELECTOR(Round011_03::showToolsSchedule));
        m_jumped = true;
        if (m_opentouched) {
            openTouch();
        }
        return;
    }
    m_pTools[m_InitAniIndex++]->runAction(ScaleTo::create(TOOLS_SCALE_TIME, 1.0f));
}

//答对后改变相应actor的动作
void Round011_03::changeAction(Actor* sender)
{
    sender->playWithSequenceAliasName(TRIGGER);
}

//庆祝
void Round011_03::celebrate(const float celebrateDelay) {
    if (m_celebrateCB) {
        m_celebrateCB(2);
    }
    
    // 播放过关庆祝动画
    m_pStageScene->getRoundUINode()->playPassActor(2);
    
    this->scheduleOnce(CC_SCHEDULE_SELECTOR(Round011_03::changeActors), celebrateDelay-2.0f);   //改变actor
}


//离开回合，缩小actor
void Round011_03::changeActors(float)
{
    for (unsigned int i = 0; i < 3; i++) {
        m_pOfficeworkers[i]->runAction(Spawn::create(ScaleTo::create(ACTORS_CHANGE_TIME, 0.8f),MoveBy::create(ACTORS_CHANGE_TIME, ACTORS_CHANGE_POS), NULL));
    }
}

/* 开启触摸 */
void Round011_03::openTouch()
{
    // 事件派发
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    // 答题监听器
    auto listenerRight = EventListenerTouchOneByOne::create();
    listenerRight->setSwallowTouches(true);
    listenerRight->onTouchBegan = [this](Touch* touch, Event* event)->bool
    {
        auto target = static_cast<Sprite*>(event->getCurrentTarget());
        auto touchPoint = target->convertTouchToNodeSpace(touch);
        Rect rect = Rect(0.0f, 0.0f, target->getContentSize().width, target->getContentSize().height);
        if (rect.containsPoint(touchPoint))
        {
            auto index = (long)target->getUserData();
            m_originalPoint = m_originalPoints[index];
            m_touchBeganPoint = touch->getLocation();
            target->setScale(1.1f);
            m_pBN->reorderChild(target, TOOLS_ZORDER + 1);
            return true;
        }
        return false;
    };
    listenerRight->onTouchMoved = [this](Touch* touch, Event* event)
    {
        auto target = static_cast<Sprite*>(event->getCurrentTarget());
        Vec2 touchMovePoint = touch->getLocation();
        Vec2 m_movePos;
        
        m_movePos.x = m_originalPoint.x + (touchMovePoint.x - m_touchBeganPoint.x);
        m_movePos.y = m_originalPoint.y + (touchMovePoint.y - m_touchBeganPoint.y);
        target->setPosition(m_movePos);
    };
    listenerRight->onTouchEnded = [this](Touch* touch, Event* event)
    {
        auto target = static_cast<Sprite*>(event->getCurrentTarget());
        unsigned long clicknumber = (unsigned long)target->getUserData();
        bool isCorrect = false;
        int curAreaNumber = -1;   //当前区域号

        m_pBN->reorderChild(target, TOOLS_ZORDER);
        target->setScale(1.0f);
        auto touchPoint = this->convertTouchToNodeSpace(touch);
        if (m_rect[0].containsPoint(touchPoint))
        {
            if (!isCorrect && clicknumber == 1) {
                isCorrect = true;
                m_correctNumber++;
                curAreaNumber = 0;
                auto dispatcher = Director::getInstance()->getEventDispatcher();
                dispatcher->removeEventListenersForTarget(target);
            }
        }
        if (m_rect[1].containsPoint(touchPoint)) {
            if (!isCorrect && clicknumber == 2) {
                isCorrect = true;
                m_correctNumber++;
                curAreaNumber = 1;
                auto dispatcher = Director::getInstance()->getEventDispatcher();
                dispatcher->removeEventListenersForTarget(target);
            }
        }
        if (m_rect[2].containsPoint(touchPoint)) {
            if (!isCorrect && clicknumber == 0) {
                isCorrect = true;
                m_correctNumber++;
                curAreaNumber = 2;
                auto dispatcher = Director::getInstance()->getEventDispatcher();
                dispatcher->removeEventListenersForTarget(target);
            }
        }
        
        if (isCorrect) {     //处理回答正确情况
            target->setPosition(m_centerRectsPos[curAreaNumber]);       //将食物放置在目标区中点
            target->runAction(FadeOut::create(TOOL_FADEOUT_TIME));      //工具渐变隐藏
            changeAction(m_pOfficeworkers[curAreaNumber]);              //改变相应actor动作
            m_pStageScene->playAnswerCorrectEffect(touch->getLocation());     //播放答对特效
            // 播放答对音效
            playAnswerCorrectEffect();
            if (getEffIndex() != -1) {
                MyAudioEngine::getInstance()->stopAudio(getEffIndex());
                //答对时主角说话
                playLeadAnsCorEff();
            }
            //答对时主角说话
            playLeadAnsCorEff();
            if (m_correctNumber == 3) {   //全部答对
                m_pStageScene->getRoundUINode()->stopReadEffect();          //停止题目音效
                m_pStageScene->getRoundUINode()->endReadTopic();            //停止读题目改变动作
                this->celebrate(CELEBRATE_DELAY);
            } else {
                if (m_celebrateCB) {            //答对庆祝
                    m_celebrateCB(1);
                }
            }
        } else {
            // 播放答错音效
            playAnswerErrorEffect();
            if (getEffIndex() != -1) {
                MyAudioEngine::getInstance()->stopAudio(getEffIndex());
            }
            //答错时主角说话
            playLeadAnsErrEff();
            if (m_anwerErrorCB) {
                m_anwerErrorCB();
            }
            target->runAction(Sequence::create(EaseBackOut::create(MoveTo::create(MOV_TIME, m_originalPoints[clicknumber])),CallFunc::create([this,target,clicknumber]{
                target->setPosition(m_originalPoints[clicknumber]);
            }), NULL));
        }
    };
    listenerRight->onTouchCancelled = [this](Touch* touch, Event* event)
    {
        auto target = static_cast<Sprite*>(event->getCurrentTarget());
        m_pBN->reorderChild(target, TOOLS_ZORDER);
        target->setScale(1.0f);
    };
    // 将本回合节点绑定到事件监听器
    dispatcher->addEventListenerWithSceneGraphPriority(listenerRight, m_pTools[0]);
    for (unsigned int i = 1; i < 3; i++) {
        dispatcher->addEventListenerWithSceneGraphPriority(listenerRight->clone(), m_pTools[i]);
    }
}

/*  回合结束回调 */
void Round011_03::endRound()
{
    if (m_completeCB) {
        m_completeCB();
    }
}
