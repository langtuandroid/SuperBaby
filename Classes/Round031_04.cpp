//
//  Round031_04.cpp
//  AbelLLExplorer
//
//  Created by Administrator on 15/6/8.
//
//

#include "Round031_04.h"
#include "Player.h"
#include "StageScene.h"
#include "StageData.h"
#include "Actor.h"
#include "MyAudioEngine.h"
#include "RoundUINode.h"
#include "CustomMessage.h"
#include "SocialShareNode.h"
#include "LeadingActor.h"
#include "ThemeSelScene.h"
#include "GafAssetCache.h"

// 庆祝延时
#define CELEBRATE_DELAY                 5.0f
#define MOV_TIME                        0.2f                                    // 答错回原位置移动时间

#define MOVE_ACTOR_TIME                 4.0f                              //分享界面ok过后主角移动时间
#define BACK_SCENE_TIME                 2.5f                              //返回关卡选择场景延时时间
#define COLORLAYER_TAG                  300
#define ANSWERS_ZORDER                  4
#define TOPIC_ZORDER                    2
#define PAOPAO_GAF_FILE                 "res/stage028/HY-GK-paopao01/HY-GK-paopao01.gaf"
#define PAOPAO_GAF_POS                  Vec2(818,868)
#define PAOPAO_GAF_ZORDER               1
#define WHITELIGHT_GAF_FILE             "res/stage028/HY-GK-guang/HY-GK-guang.gaf"
#define WHITELIGHT_GAF_POS              Vec2(872,795)
#define WHITELIGHT_GAF_ZORDER           1
#define HINT_POS                        Vec2(1285,156)
#define HINT_PNG_FILE                   "HY-GK07-4-TM01.png"
#define HIGHLIGHT_HINT_PNG_FILE         "HY-GK07-4-TM02.png"
#define HINT_ZORDER                     2
#define ANSWER_PNG_FILE                 "HY-GK07-4-DA01.png"
/*
 *  constructor
 */
Round031_04::Round031_04()
{
    m_answersPos[0] = Vec2(593, 971);
    m_answersPos[1] = Vec2(1110, 971);
    m_answersPos[2] = Vec2(1627, 971);
}

/*
 *  destructor
 */
Round031_04::~Round031_04()
{
    cleanUp();
}

//
Round031_04* Round031_04::createWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    auto pRound = new Round031_04();
    if (pRound && pRound->initWithRoundIndex(roundIndex, pStageScene,pos))
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
bool Round031_04::initWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    m_roundIndex = roundIndex;
    m_pStageScene = pStageScene;
    
    setAnchorPoint(Vec2::ZERO);
    setPosition(pos);
    
    m_pPaopaoActor = Actor::createWithGafFile(PAOPAO_GAF_FILE);
    m_pPaopaoActor->setPosition(PAOPAO_GAF_POS);
    this->addChild(m_pPaopaoActor, PAOPAO_GAF_ZORDER);
    
    m_pWhiteLightActor = Actor::createWithGafFile(WHITELIGHT_GAF_FILE);
    m_pWhiteLightActor->setPosition(WHITELIGHT_GAF_POS);
    m_pWhiteLightActor->setSize(Size(300, 300));
    m_pWhiteLightActor->setPosAtNode(Vec2(-300, 400));
    this->addChild(m_pWhiteLightActor, WHITELIGHT_GAF_ZORDER);
    
    char buffer[64];
    for (unsigned long i = 0; i < 3; i++) {
        sprintf(buffer, "HY-GK07-4-DA%02ld.png",i+2);
        m_pAnswersImageView[i] = ImageView::create(buffer,ui::Widget::TextureResType::PLIST);
        m_pAnswersImageView[i]->setUserData((void*)i);
        this->addChild(m_pAnswersImageView[i], ANSWERS_ZORDER);
        if (i%2 == 0) {
            auto repeat = RepeatForever::create(Sequence::create(MoveBy::create(0.5f, Vec2(0.0f, -10.f)),MoveBy::create(0.5f, Vec2(0.0f, 10.0f)), NULL));
            repeat->setTag(100);
            m_pAnswersImageView[i]->runAction(repeat);
        }else
        {
            auto repeat = RepeatForever::create(Sequence::create(MoveBy::create(0.5f, Vec2(0.0f, 10.f)),MoveBy::create(0.5f, Vec2(0.0f, -10.0f)), NULL));
            repeat->setTag(100);
            m_pAnswersImageView[i]->runAction(repeat);
        }
    }
    
    m_pHintSp = Sprite::createWithSpriteFrameName(HINT_PNG_FILE);
    m_pHintSp->setPosition(HINT_POS);
    this->addChild(m_pHintSp, HINT_ZORDER);
    
    m_pHighLightHintSp = Sprite::createWithSpriteFrameName(HIGHLIGHT_HINT_PNG_FILE);
    m_pHighLightHintSp->setPosition(HINT_POS);
    this->addChild(m_pHighLightHintSp, HINT_ZORDER);
    m_pHighLightHintSp->setVisible(false);
    
    unsigned int all = 0;
    bool selected[3] = {false, false, false};
    srand(unsigned(time(0)));
    while (all < 3) {
        unsigned int randIndex = static_cast<unsigned int>(rand() % 3);
        if (selected[randIndex]) continue;
        selected[randIndex] = true;
        m_pAnswersImageView[all]->setPosition(m_answersPos[randIndex]);
        m_answersEndPos[all] = m_answersPos[randIndex];
        all++;
    } /*while*/

    return true;
}

void Round031_04::onPreLoadedRes()
{
    /////
    RoundBase::onPreLoadedRes();
    
}

// 进入本回合
void Round031_04::onEnterRound()
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
    dispatcher->addCustomEventListener(SHARE_CALLBACK, [this] (EventCustom *) {
        auto dispatcher = Director::getInstance()->getEventDispatcher();
        dispatcher->removeCustomEventListeners(SHARE_CALLBACK);
        shareCallback();
    });
    changeScaleHint();
}

//放大答案以提示
void Round031_04::changeScaleHint()
{

    m_pHintSp->runAction(Sequence::create(ScaleTo::create(0.4f, 1.05f),
                                                           CallFunc::create([this]{
        m_pHintSp->setVisible(false);
        m_pHighLightHintSp->setVisible(true);
    }),
                                                           DelayTime::create(0.2f),
                                                           CallFunc::create([this]{
        m_pHighLightHintSp->setVisible(false);
        m_pHintSp->setVisible(true);
    }),
                                                           ScaleTo::create(0.4f, 1.0f),
                                                           CallFunc::create([this]{
        m_jumped = true;
        if (m_opentouched) {
            openTouch();
        }
    }),NULL));
}

//
void Round031_04::cleanUp()
{
    m_pStageScene->removeOBAllNode(COLORLAYER_TAG);
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->removeCustomEventListeners("openTouch");
    dispatcher->removeCustomEventListeners("endRound");
    dispatcher->removeCustomEventListeners(SHARE_CALLBACK);
    GafAssetCache::getInstance()->removeGafAssetForKey(PAOPAO_GAF_FILE);
    GafAssetCache::getInstance()->removeGafAssetForKey(WHITELIGHT_GAF_FILE);
}

/* 开启触摸 */
void Round031_04::openTouch()
{
    ///////////////////
    // 事件派发
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    auto listenerDrag = EventListenerTouchOneByOne::create();
    listenerDrag->setSwallowTouches(true);
    listenerDrag->onTouchBegan = [this](Touch* touch, Event* event) -> bool
    {
        auto target = static_cast<ImageView*>(event->getCurrentTarget());
        auto touchPoint = target->convertTouchToNodeSpace(touch);
        Rect rect = Rect(0.0f, 0.0f, target->getContentSize().width, target->getContentSize().height);
        if (rect.containsPoint(touchPoint))
        {
            target->setLocalZOrder(5);
            target->setScale(1.1f);
            m_originalPoint = target->getPosition();
            m_touchBeganPoint = touch->getLocation();
            return true;
        }
        return false;
    };
    listenerDrag->onTouchMoved = [this](Touch* touch, Event* event)
    {
        auto target = static_cast<ImageView*>(event->getCurrentTarget());
        Vec2 touchMovePoint = touch->getLocation();
        Vec2 m_movePos;
        
        m_movePos.x = m_originalPoint.x + (touchMovePoint.x - m_touchBeganPoint.x);
        m_movePos.y = m_originalPoint.y + (touchMovePoint.y - m_touchBeganPoint.y);
        target->setPosition(m_movePos);
    };
    listenerDrag->onTouchEnded = [this](Touch* touch, Event* event)
    {
        auto target = static_cast<ImageView*>(event->getCurrentTarget());
        auto touchPoint = m_pHintSp->convertTouchToNodeSpace(touch);
        long index = (long)target->getUserData();
        target->setScale(1.0f);
        Rect rect = Rect(0.0f, 0.0f,
                         m_pHintSp->getContentSize().width,
                         m_pHintSp->getContentSize().height);
        if (rect.containsPoint(touchPoint))
        {
            if (index == 0) {
                target->stopActionByTag(100);
                m_pHintSp->setVisible(false);
                target->setPosition(HINT_POS);
                target->loadTexture(ANSWER_PNG_FILE,ui::Widget::TextureResType::PLIST);
                // 播放答对特效
                m_pStageScene->playAnswerCorrectEffect(touch->getLocation());
                // 播放答对音效
                playAnswerCorrectEffect();
                
                if (getEffIndex() != -1) {
                    MyAudioEngine::getInstance()->stopAudio(getEffIndex());
                }
                //答对时主角说话
                playLeadAnsCorEff();
                
                m_pStageScene->getRoundUINode()->stopReadEffect();          //停止题目音效
                m_pStageScene->getRoundUINode()->endReadTopic();            //停止读题目改变动作
                celebrate(CELEBRATE_DELAY);
                fadeAnswers();
                for (unsigned int i = 0; i < 3; i++) {
                    Director::getInstance()->getEventDispatcher()->removeEventListenersForTarget(m_pAnswersImageView[i]);
                } /*for*/
            }else
            {
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
                target->runAction(Sequence::create(EaseBackOut::create(MoveTo::create(MOV_TIME, m_answersEndPos[index])),CallFunc::create([this,target,index]{
                    target->setLocalZOrder(ANSWERS_ZORDER);
                    target->setPosition(m_answersEndPos[index]);
                }), NULL));
            }
        }else
        {
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
            target->runAction(Sequence::create(EaseBackOut::create(MoveTo::create(MOV_TIME, m_answersEndPos[index])),CallFunc::create([this,target,index]{
                target->setLocalZOrder(ANSWERS_ZORDER);
                target->setPosition(m_answersEndPos[index]);
            }), NULL));
        }
    };
    listenerDrag->onTouchCancelled = [this](Touch* touch, Event* event)
    {
        auto target = static_cast<Sprite*>(event->getCurrentTarget());
        long index = (long)target->getUserData();
        target->setPosition(m_answersEndPos[index]);
        target->setLocalZOrder(ANSWERS_ZORDER);
    };
    dispatcher->addEventListenerWithSceneGraphPriority(listenerDrag, m_pAnswersImageView[0]);
    for (unsigned int i = 1; i < 3; i++) {
        dispatcher->addEventListenerWithSceneGraphPriority(listenerDrag->clone(), m_pAnswersImageView[i]);
    } /*for*/
}

//隐藏剩余答案
void Round031_04::fadeAnswers()
{
    for (unsigned int i = 0; i < 3; i++) {
        if (i != 0) {
            m_pAnswersImageView[i]->runAction(FadeOut::create(0.3f));
        }
    }
}

/*
 *  庆祝
 */
void Round031_04::celebrate(const float celebrateDelay /*庆祝延时*/ )
{
    //播放关卡结束分享音效
    auto pStageData = m_pStageScene->getStageData();
    m_effectId = MyAudioEngine::getInstance()->playAudio(pStageData->getShareAudio().c_str());
    
    if (m_celebrateCB) {
        m_celebrateCB(1);
    }
    m_pStageScene->getRoundUINode()->playPassActor(3);
}

/*  回合结束回调 */
void Round031_04::endRound() {}

/*
 *  分享界面点击ok后移动主角
 */
void Round031_04::moveActor()
{
    
    m_pStageScene->getBeibeiActor()->walk(); m_pStageScene->getAimeeActor()->walk();
    m_pStageScene->moveLeadingActor(MOVE_ACTOR_TIME, Vec2(2000, 0));

}

//回合结束分享调用
void Round031_04::roundEndShareCallBack(bool isShare)
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
    moveActor();
    scheduleOnce(CC_SCHEDULE_SELECTOR(Round031_04::fadeOutLayer), 3.0f);
}

//渐变变黑退出
void Round031_04::fadeOutLayer(float)
{
    auto size = Director::getInstance()->getVisibleSize();
    m_LayerColor = LayerColor::create(Color4B(0, 0, 0, 0), size.width, size.height);
    m_pStageScene->addOverAllNode(m_LayerColor, COLORLAYER_TAG);
    this->schedule([this](float dt){
        if (m_opacity == 200) {
            unschedule("add");
            MyAudioEngine::getInstance()->stopAudio(m_effectId);    //停止分享音效
            if (m_pStageScene->isTrial())
            {
                Director::getInstance()->replaceScene(ThemeSelScene::create());
            }else
            {
                // 过关
                m_pStageScene->stageClear();
                //试玩时停止播放背景音乐
                if (m_pStageScene->isTrial())
                {
                    MyAudioEngine::getInstance()->stopAudio(0,true);
                }
            }
            
            return;
        }
        m_LayerColor->setOpacity(m_opacity++);
    }, 0.001f, "add");
}

//分享回调
void Round031_04::shareCallback()
{
    m_pStageScene->getRoundUINode()->getSocialShareNode()->setOKCb(std::bind(&Round031_04::roundEndShareCallBack, this,false));
    m_pStageScene->getRoundUINode()->getSocialShareNode()->setCloseCb(std::bind(&Round031_04::roundEndShareCallBack, this,true));
}