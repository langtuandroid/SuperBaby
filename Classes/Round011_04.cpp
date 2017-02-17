//
//  Round011_04.cpp
//  AbelLLExplorer
//
//  Created by Administrator on 2015/4/9.
//
//

#include "Round011_04.h"
#include "StageScene.h"
#include "StageData.h"
#include "Actor.h"
#include "SocialShareNode.h"
#include "LeadingActor.h"
#include "MyAudioEngine.h"
#include "RoundUINode.h"
#include "CustomMessage.h"

#define CELEBRATE_DELAY                  5.0f                                    // 庆祝延时
#define BN_PNG_FILE                    "res/stage011/SL-GK11-IT.png"             //BN的png文件
#define BN_ZORDER                      2                                         //BN的z值
#define STANDBY                          "standby"                               // 待机动画
#define TRIGGER                          "trigger"                               // 触发动画
#define MOVE_ACTOR_TIME               3.0f                              //分享界面ok过后主角移动时间
#define EQUATION_ONE_POS              Vec2(1027,1789)                    //未完成等式一位置
#define EQUATION_TWO_POS              Vec2(1027,2000)                    //未完成等式二位置
#define EQUATION_PASS_ONE_POS         Vec2(1027,789)                    //完成的等式一位置
#define EQUATION_PASS_TWO_POS         Vec2(1027,1000)                    //完成的等式二位置
#define EQUATION__ZORDER              1                                 //未完成等式z值
#define EQUATION_PASS_ZORDER          1                                 //完成等式z值
#define ANSWER_ZORDER                 2                                 //答案z值
#define DOWN_MOVE_TIME                1.0f                              //等式下落时间
#define DOWN_MOVE_OFFSETY             -1000                             //等式下降高度
#define UP_MOVE_TIME                  1.0f                              //等式上升时间
#define UP_MOVE_OFFSETY               1000                             //等式上升高度
#define LEFT_MOVE_TIME                0.6f                              //答案左移时间
#define LEFT_MOVE_OFFSETX             -1200                             //答案左移宽度
#define MOV_TIME                       0.2f                             //答错回原位置移动时间
#define FADE_OUT_TIME                  0.5f                              //到达目标区域渐变消失时间
#define HIPPO_GAF_CONFIG_FILE         "config/stage011/hippo.json"      //河马gaf的配置文件
#define HIPPO_GAF_POS                 Vec2(-390,900)                     //河马的坐标
#define HIPPO_GAF_ZORDER              1                                 //河马z值
#define COLORLAYER_TAG                  300

/*
 *  constructor
 */
Round011_04::Round011_04()
{
    //设置答案坐标
    m_answersPos[0] = Vec2(2165, 502);
    m_answersPos[1] = Vec2(2405, 502);
    
    //设置触摸响应区
    m_rect[0] = Rect(724, 689, 200, 200);
    m_rect[1] = Rect(724, 900, 200, 200);
}

/*
 *  destructor
 */
Round011_04::~Round011_04()
{
    m_pStageScene->removeOBAllNode(COLORLAYER_TAG);
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->removeCustomEventListeners("openTouch");
    dispatcher->removeCustomEventListeners(SHARE_CALLBACK);
}

//
Round011_04* Round011_04::createWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    auto pRound = new Round011_04();
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
bool Round011_04::initWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    m_pStageScene = pStageScene;
    m_roundIndex = roundIndex;
    
    setAnchorPoint(Vec2::ZERO);
    setPosition(pos);
    
    appendAsyncLoadGaf("res/stage011/SL-GK11-4-MS01-C+CC/SL-GK11-4-MS01-C+CC.gaf");
    preLoadRes();
    return true;
}

int Round011_04::randomIndex(int min,int max)
{
    srand(unsigned(time(0)));
    return random(min, max);
}

bool Round011_04::numberIsRepeat(std::vector<int>& indexs,int number)
{
    while (true) {
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

int Round011_04::getRandomNumber(std::vector<int>& indexs,int min, int max)
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
void Round011_04::onPreLoadedRes()
{
    ///////
    RoundBase::onPreLoadedRes();
    
    //加载河马
    m_pHippoActor = Actor::createWithGafConfigFile(HIPPO_GAF_CONFIG_FILE);
    m_pHippoActor->setPosition(HIPPO_GAF_POS);
    m_pHippoActor->playWithSequenceAliasName(STANDBY);
    addChild(m_pHippoActor, HIPPO_GAF_ZORDER);
    
    // 纹理加载
    m_pBN = SpriteBatchNode::create(BN_PNG_FILE);
    addChild(m_pBN, BN_ZORDER);
    
    //加载等式
    char dsBuffer[64];
    for (unsigned int i = 0; i < 2; i++) {
        if (i == 0) {
            sprintf(dsBuffer, "SL-GK11-4-TM02-D.png");
            m_pEquation[i] = Sprite::createWithSpriteFrameName(dsBuffer);
            m_pEquation[i]->setPosition(EQUATION_ONE_POS);
        }else
        {
            sprintf(dsBuffer, "SL-GK11-4-TM01-D.png");
            m_pEquation[i] = Sprite::createWithSpriteFrameName(dsBuffer);
            m_pEquation[i]->setPosition(EQUATION_TWO_POS);
        }
        m_pBN->addChild(m_pEquation[i], EQUATION__ZORDER);
    }
    
    //加载答对等式
    for (unsigned int i = 0; i < 2; i++) {
        if (i == 0) {
            sprintf(dsBuffer, "SL-GK11-4-TM02-C.png");
             m_pPassEquation[i] = Sprite::createWithSpriteFrameName(dsBuffer);
            m_pPassEquation[i]->setPosition(EQUATION_PASS_ONE_POS);
        }else
        {
            sprintf(dsBuffer, "SL-GK11-4-TM01-C.png");
            m_pPassEquation[i] = Sprite::createWithSpriteFrameName(dsBuffer);
            m_pPassEquation[i]->setPosition(EQUATION_PASS_TWO_POS);
        }
        m_pPassEquation[i]->setVisible(false);
        m_pBN->addChild(m_pPassEquation[i], EQUATION_PASS_ZORDER);
    }
    
    //加载答案
    for (unsigned long i = 0; i < 2; i++) {
        sprintf(dsBuffer, "SL-GK11-4-DA%02ld.png",i+1);
        m_pAnswers[i] = Sprite::createWithSpriteFrameName(dsBuffer);
        auto index = getRandomNumber(m_pCreatedIndexs, 0, 1);  //获得随机
        m_pAnswers[i]->setPosition(m_answersPos[index]);
        m_answerEndPos[i] = m_answersPos[index];
        m_answerEndPos[i] = Vec2(m_answersPos[index].x-1200, m_answersPos[index].y);
        m_pAnswers[i]->setUserData((void*)i);
        m_pBN->addChild(m_pAnswers[i], ANSWER_ZORDER);
    }
    m_pCreatedIndexs.clear();
}

//开始回合
void Round011_04::onEnterRound()
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
    dispatcher->addCustomEventListener(SHARE_CALLBACK, [this] (EventCustom *) {
        auto dispatcher = Director::getInstance()->getEventDispatcher();
        dispatcher->removeCustomEventListeners(SHARE_CALLBACK);
        shareCallback();
    });
    this->schedule(CC_SCHEDULE_SELECTOR(Round011_04::downMoveEquationSchedule), DOWN_MOVE_TIME, -1, 0);
    
}

//回合开始，等式一从上部出现
void Round011_04::downMoveEquationSchedule(float)
{
    if (m_InitAniIndex >= 2) {
        unschedule(CC_SCHEDULE_SELECTOR(Round011_04::downMoveEquationSchedule));
        m_InitAniIndex = 0;        //重置为0
        //答案左移
        this->schedule(CC_SCHEDULE_SELECTOR(Round011_04::leftMoveAnswersSchedule), LEFT_MOVE_TIME, -1, 0);
        return;
    }
    m_pEquation[m_InitAniIndex++]->runAction(MoveBy::create(DOWN_MOVE_TIME, Vec2(0.0f, DOWN_MOVE_OFFSETY)));
}

//回合开始，答案右边出现
void Round011_04::leftMoveAnswersSchedule(float)
{
    if (m_InitAniIndex >= 2) {
        unschedule(CC_SCHEDULE_SELECTOR(Round011_04::downMoveEquationSchedule));
        m_jumped = true;
        if (m_opentouched) {
            openTouch();
        }
        return;
    }
    m_pAnswers[m_InitAniIndex++]->runAction(MoveBy::create(LEFT_MOVE_TIME, Vec2(LEFT_MOVE_OFFSETX, 0.0f)));
}

//答对改变犀牛actor动作
void Round011_04::changeHippoActor()
{
    m_pHippoActor->playWithSequenceAliasName(TRIGGER,false);
}

//庆祝
void Round011_04::celebrate(const float celebrateDelay) {
    
    //播放关卡结束分享音效
    auto pStageData = m_pStageScene->getStageData();
    m_effectId = MyAudioEngine::getInstance()->playAudio(pStageData->getShareAudio().c_str());
    if (m_celebrateCB) {
        m_celebrateCB(2);
    }
    
    // 播放过关庆祝动画
    m_pStageScene->getRoundUINode()->playPassActor(3);
    
}

//回合结束，上升完成的等式
void Round011_04::upMoveEquationSchedule(float)
{
    if (m_InitAniIndex <= -1) {
        unschedule(CC_SCHEDULE_SELECTOR(Round011_04::upMoveEquationSchedule));
        return;
    }
    m_pPassEquation[m_InitAniIndex--]->runAction(MoveBy::create(UP_MOVE_TIME, Vec2(0.0f, UP_MOVE_OFFSETY)));
}

//回合结束分享调用
void Round011_04::roundEndShareCallBack(bool isShare)
{
    if (isShare) {
        auto shareBackGround = m_pStageScene->getRoundUINode()->getSocialShareNode()->getShareBackGround();
        shareBackGround->runAction(Sequence::create(Spawn::create(EaseBackIn::create(ScaleTo::create(0.3f, 0.0f)),FadeOut::create(0.5f), NULL),CallFunc::create([this]{
            m_pStageScene->getRoundUINode()->getSocialShareNode()->removeFromParent();
        }), NULL));
    }else
    {
        auto layer = m_pStageScene->getRoundUINode()->getSocialShareNode()->getLayer();
        layer->runAction(Sequence::create(Spawn::create(EaseBackIn::create(ScaleTo::create(0.3f, 0.0f)),FadeOut::create(0.5f), NULL),CallFunc::create([this]{
            m_pStageScene->getRoundUINode()->getSocialShareNode()->removeFromParent();
        }), NULL));
    }
    m_InitAniIndex = 1;                 //重置为1
    this->schedule(CC_SCHEDULE_SELECTOR(Round011_04::upMoveEquationSchedule), 0.3f, -1, 0);
    this->scheduleOnce(CC_SCHEDULE_SELECTOR(Round011_04::moveActor), UP_MOVE_TIME+3.0f);    //移动主角
}

/* 开启触摸 */
void Round011_04::openTouch()
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
            m_originalPoint = target->getPosition();
            m_touchBeganPoint = touch->getLocation();
            target->setScale(1.1f);
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
        auto touchPoint = this->convertTouchToNodeSpace(touch);
        Rect rect = Rect(0.0f, 0.0f, target->getContentSize().width, target->getContentSize().height);
        target->setScale(1.0f);
        unsigned long clicknumber = (unsigned long)target->getUserData();
        bool isCorrect = false;
        int curAreaNumber = -1;   //当前区域号
        if (m_rect[0].containsPoint(touchPoint)) {
            if (!isCorrect && clicknumber == 1) {
                isCorrect = true;
                m_correctNumber++;
                curAreaNumber = 0;
                auto dispatcher = Director::getInstance()->getEventDispatcher();
                dispatcher->removeEventListenersForTarget(target);
            }
        }
        if (m_rect[1].containsPoint(touchPoint)) {
            if (!isCorrect && clicknumber == 0) {
                isCorrect = true;
                m_correctNumber++;
                curAreaNumber = 1;
                auto dispatcher = Director::getInstance()->getEventDispatcher();
                dispatcher->removeEventListenersForTarget(target);
            }
        }
        
        if (isCorrect) {     //处理回答正确情况
            target->runAction(FadeOut::create(FADE_OUT_TIME));      //符号渐变隐藏
            m_pEquation[curAreaNumber]->setVisible(false);              //隐藏题目
            m_pPassEquation[curAreaNumber]->setVisible(true);           //显示答对状态题目
            m_pStageScene->playAnswerCorrectEffect(touch->getLocation());     //播放答对特效
            // 播放答对音效
            playAnswerCorrectEffect();
            if (getEffIndex() != -1) {
                MyAudioEngine::getInstance()->stopAudio(getEffIndex());
            }
            //答对时主角说话
            playLeadAnsCorEff();
            if (m_correctNumber == 2) {   //全部答对
                changeHippoActor();                                          //改变相应actor动作
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
            target->runAction(Sequence::create(EaseBackOut::create(MoveTo::create(MOV_TIME, m_answerEndPos[clicknumber])),CallFunc::create([this,target,clicknumber]{
                target->setPosition(m_answerEndPos[clicknumber]);
            }), NULL));
        }
    };
    // 将本回合节点绑定到事件监听器
    dispatcher->addEventListenerWithSceneGraphPriority(listenerRight, m_pAnswers[0]);
    for (unsigned int i = 1; i < 2; i++) {
        dispatcher->addEventListenerWithSceneGraphPriority(listenerRight->clone(), m_pAnswers[i]);
    }
}

/*  回合结束回调 */
void Round011_04::endRound() {}

//移动主角退出回合
void Round011_04::moveActor(float)
{
    m_pStageScene->moveLeadingActor(MOVE_ACTOR_TIME, Vec2(2000.0f, 0.0f), true);
    this->scheduleOnce(CC_SCHEDULE_SELECTOR(Round011_04::fadeOutLayer), MOVE_ACTOR_TIME + 0.1f);    //退出
}

//渐变变黑退出
void Round011_04::fadeOutLayer(float)
{
    MyAudioEngine::getInstance()->stopAudio(m_effectId);    //停止分享音效
   // 过关
    m_pStageScene->stageClear();
}

//分享回调
void Round011_04::shareCallback()
{
    m_pStageScene->getRoundUINode()->getSocialShareNode()->setOKCb(std::bind(&Round011_04::roundEndShareCallBack, this, false));
    m_pStageScene->getRoundUINode()->getSocialShareNode()->setCloseCb(std::bind(&Round011_04::roundEndShareCallBack, this, true));
}