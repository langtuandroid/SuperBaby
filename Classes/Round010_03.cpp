//
//  Round010_03.cpp
//  AbelLLExplorer
//
//  Created by Administrator on 2015/4/8.
//
//

#include "Round010_03.h"
#include "StageScene.h"
#include "StageData.h"
#include "Actor.h"
#include "MyAudioEngine.h"
#include "RoundUINode.h"
#include "AppConfig.h"

#define CELEBRATE_DELAY                  5.0f                                    // 庆祝延时
#define BN_PNG_FILE                    "res/stage010/SL-GK10-IT.png"             //BN的png文件
#define BN_ZORDER                      2                                         //BN的z值
#define FadeOut_TIME                  1                                         //动物图淡出时间
#define SCALE_TIME                    1                                         //动物图变小时间
#define STANDBY                        "standby"                                 // 待机动画
#define TRIGGER                        "trigger"                                 // 触发动画
#define MONSTER_GAF_CONFIG_FILE        "config/stage010/monster.json"            //怪物gaf配置文件
#define MONSTER_GAF_ZORDER             1                                        //怪物z值
#define MONSTER_GAF_POS                Vec2(596,1283)                           //怪物位置
#define ANIMAL_ZORDER                  1                                        //动物图z值
#define ANIMAL_MOVE_TIME               1.0f                                     //动物右移时间
#define ANIMAL_MOVE_OFFSETX            1800                                     //动物图右移宽度
#define ANIMAL_CHANGE_SCALE_TIME       0.5f                                     //动物图大小改变时间
#define TOUCH_EFFECT_GAF_FILE          "res/common/SL-dianjichufa/SL-dianjichufa.gaf"    //点击效果gaf
#define TOUCH_EFFECT_GAF_ZORDER        5                                        //点击效果z值
#define RED_ANIMAL_ZORDER              2                                        //红色动物图z值

/*
 *  constructor
 */
Round010_03::Round010_03()
:m_InitAniIndex(0),
m_correctNumber(0)
{
   //怪物身体部位坐标组
   m_partsPos[0] = Vec2(1756,353);   //尾巴
   m_partsPos[1] = Vec2(1480,583);   //头部
   m_partsPos[2] = Vec2(1616,303);   //脚
}

/*
 *  destructor
 */
Round010_03::~Round010_03() {
    cleanUp();
}

/*
 *  清理
 */

void Round010_03::cleanUp()
{
   auto dispatcher = Director::getInstance()->getEventDispatcher();
   dispatcher->removeCustomEventListeners("endRound");
}

void Round010_03::onPreLoadedRes()
{
    RoundBase::onPreLoadedRes();
    
    //纹理加载
    m_pBN = SpriteBatchNode::create(BN_PNG_FILE);
    addChild(m_pBN, BN_ZORDER);
    
    // 加载怪物actor
    m_pMonsterActor = Actor::createWithGafConfigFile(MONSTER_GAF_CONFIG_FILE);
    m_pMonsterActor->setPosition(MONSTER_GAF_POS);
    m_pMonsterActor->playWithSequenceAliasName(STANDBY,true);
    addChild(m_pMonsterActor, MONSTER_GAF_ZORDER);
   
}

//
Round010_03* Round010_03::createWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
   auto pRound = new Round010_03();
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
bool Round010_03::initWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
   m_pStageScene = pStageScene;
   m_roundIndex = roundIndex;
   
    setAnchorPoint(Vec2::ZERO);
    setPosition(pos);
    
    appendAsyncLoadGaf("res/stage010/SL-GK10-3-TM01-D+C/SL-GK10-3-TM01-D+C.gaf");
    preLoadRes();
    
    return true;
}

//开始回合
void Round010_03::onEnterRound()
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
    //加载动物图
    float dwX = -1496; float offSetX = 306; float dwY = 963;
    char dwBuffer[64];
    for (unsigned long i = 0; i < 4; i++) {
        sprintf(dwBuffer, "SL-GK010-3-DA%02ld.png",i+1);
        m_pAnimals[i] = Sprite::createWithSpriteFrameName(dwBuffer);
        m_pAnimals[i]->setUserData((void*)i);
        m_pBN->addChild(m_pAnimals[i], ANIMAL_ZORDER);
    }
   //随机
   unsigned int all = 0;
   bool selected[4] = {false, false, false, false};
   srand(unsigned(time(0)));
   while (all < 4) {
      unsigned int Index = static_cast<unsigned int>(rand() % 4);
      if (selected[Index]) continue;
      selected[Index] = true;
      m_pAnimals[all]->setPosition(Vec2(dwX+offSetX*Index, dwY));
      all++;
   } /*while*/
   
    this->schedule(CC_SCHEDULE_SELECTOR(Round010_03::animalMoveSchedule), ANIMAL_MOVE_TIME - 0.5f, -1, 0.0f);  //上升数字
}

//回合开始，动物图右移出现
void Round010_03::animalMoveSchedule(float)
{
    if (m_InitAniIndex >= 4) {
        unschedule(CC_SCHEDULE_SELECTOR(Round010_03::animalMoveSchedule));
        m_jumped = true;
        if (m_opentouched) {
           openTouch();
        }
        return;
    }
    m_pAnimals[m_InitAniIndex++]->runAction(MoveBy::create(ANIMAL_MOVE_TIME, Vec2(ANIMAL_MOVE_OFFSETX, 0.0f)));
}
//答对之后，动物图变小渐变消失
void Round010_03::animalFadeOut(int index)
{
    m_pAnimals[index]->runAction(Spawn::create(ScaleTo::create(SCALE_TIME, 0.0f),FadeOut::create(FadeOut_TIME-0.5f), NULL));
}

//庆祝
void Round010_03::celebrate(const float celebrateDelay) {
    
   this->scheduleOnce(CC_SCHEDULE_SELECTOR(Round010_03::changeMonsterActor), 1.0f);           //改变怪物actor动作
    if (m_celebrateCB) {
        m_celebrateCB(true);
    }
    
   // 播放过关庆祝动画
   m_pStageScene->getRoundUINode()->playPassActor(2);
   
}


//回合结束，怪物actor改变动作
void Round010_03::changeMonsterActor(float dt)
{
    m_pMonsterActor->playWithSequenceAliasName(TRIGGER);
}

/* 开启触摸 */
void Round010_03::openTouch()
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
         target->setScale(1.1f);
         return true;
      }
      return false;
   };
   listenerRight->onTouchEnded = [this](Touch* touch, Event* event)
   {
      auto target = static_cast<Sprite*>(event->getCurrentTarget());
      target->setScale(1.0f);
      unsigned long clicknumber = (unsigned long)target->getUserData();
       auto touchPoint = target->convertTouchToNodeSpace(touch);
       Rect rect = Rect(0.0f, 0.0f, target->getContentSize().width, target->getContentSize().height);
       if (rect.containsPoint(touchPoint))
      {
         if (clicknumber == 0 ||clicknumber == 1 ||clicknumber == 3) { //答对问题
            animalFadeOut((int)clicknumber);                          //动物图变小渐变消失
            auto offSetX = AppConfig::getInstance()->getSceneOffsetX();
            if (clicknumber == 3) {
               m_pStageScene->playAnswerCorrectEffect(Vec2(m_partsPos[clicknumber-1].x+offSetX, m_partsPos[clicknumber-1].y));     //播放答对特效
            }else
            {
               m_pStageScene->playAnswerCorrectEffect(Vec2(m_partsPos[clicknumber].x+offSetX, m_partsPos[clicknumber].y));     //播放答对特效
            }
            // 播放答对音效
            playAnswerCorrectEffect();
            if (getEffIndex() != -1) {
               MyAudioEngine::getInstance()->stopAudio(getEffIndex());
            }
            //答对时主角说话
            playLeadAnsCorEff();
            m_correctNumber++;
            auto dispatcher = Director::getInstance()->getEventDispatcher();
            dispatcher->removeEventListenersForTarget(target);
         } else //答错
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
         }
      }
      
      if (m_correctNumber == 3) {
         auto dispatcher = Director::getInstance()->getEventDispatcher();
         dispatcher->removeEventListenersForTarget(m_pAnimals[2]);
         m_pStageScene->getRoundUINode()->stopReadEffect();          //停止题目音效
         m_pStageScene->getRoundUINode()->endReadTopic();            //停止读题目改变动作
         this->celebrate(CELEBRATE_DELAY);
         for (unsigned int i = 0; i < 4; i++) {
            Director::getInstance()->getEventDispatcher()->removeEventListenersForTarget(m_pAnimals[i]);
         } /*for*/
      }
   };
   // 将本回合节点绑定到事件监听器
   dispatcher->addEventListenerWithSceneGraphPriority(listenerRight, m_pAnimals[0]);
   for (unsigned int i = 1; i < 4; i++) {
      dispatcher->addEventListenerWithSceneGraphPriority(listenerRight->clone(), m_pAnimals[i]);
   }
}

/*  回合结束回调 */
void Round010_03::endRound()
{
   animalFadeOut(2);               //消除剩余的动物图
   if (m_completeCB) {
      m_completeCB();
   }
}
