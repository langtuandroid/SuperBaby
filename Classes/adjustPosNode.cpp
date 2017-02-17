//
//  adjustPosNode.cpp
//  AbelLLExplorer
//
//  Created by adinnet on 2015/5/4.
//
//

#include "adjustPosNode.h"
#include "Actor.h"
#include "GAFPlayer/GAF.h"
#include "ui/CocosGUI.h"

#define BUTTON_IMG          "res/common/npcArrow.png"

//
adjustPosNode::adjustPosNode() {}

//
adjustPosNode::~adjustPosNode() {}

//
adjustPosNode* adjustPosNode::createWithNode(cocos2d::Node *pNode)
{
    auto pAdjustPosNode = new adjustPosNode();
    if (pAdjustPosNode && pAdjustPosNode->initWithNode(pNode)) {
        pAdjustPosNode->autorelease();
    } else {
        CC_SAFE_DELETE(pAdjustPosNode);
    }
    return pAdjustPosNode;
}

//
bool adjustPosNode::initWithNode(cocos2d::Node *pNode)
{
    m_pNode = pNode;
    auto origin = Director::getInstance()->getVisibleOrigin();
    auto visibleSize = Director::getInstance()->getVisibleSize();
    
    auto btn1 = ui::Button::create(BUTTON_IMG);
    auto width = btn1->getContentSize().width;
    auto height = btn1->getContentSize().height;
    btn1->setRotation( 90 );
    btn1->setPosition(Vec2(width / 2.0f, height / 2.0f));
    addChild(btn1, 1, 1);
    
    auto btn2 = ui::Button::create(BUTTON_IMG);
    btn2->setPosition(Vec2(width + width / 2.0f, height / 2.0f));
    addChild(btn2, 1, 2);
    
    auto btn3 = ui::Button::create(BUTTON_IMG);
    btn3->setRotation( -90 );
    btn3->setPosition(Vec2(width * 2 + width / 2.0f, height / 2.0f));
    addChild(btn3, 1, 3);
    
    auto btn4 = ui::Button::create(BUTTON_IMG);
    btn4->setFlippedY( true );
    btn4->setPosition(Vec2(width + width / 2.0f, height / 2.0f + height));
    addChild(btn4, 1, 4);
    
    auto pArraw = ui::ImageView::create( BUTTON_IMG );
    pArraw->setPosition( Vec2(origin.x + visibleSize.width*0.5f, origin.y + visibleSize.height*0.5f) );
    pArraw->setRotation( 0 );
    addChild( pArraw, 1, 99 );
    
    btn1->addTouchEventListener([this, pArraw](Ref* pSender, ui::Widget::TouchEventType type)
                                {
                                    if( type == ui::Widget::TouchEventType::BEGAN )
                                    {
                                        this->scheduleUpdate(); m_fPressedTime = 0.0f;
                                        m_curSelectBtn = dynamic_cast<Node*>( pSender )->getTag();
                                    }
                                    else if (type == ui::Widget::TouchEventType::ENDED)
                                    {
                                        if (m_pNode != nullptr) {
                                            auto pos = m_pNode->getPosition();
                                            pos.x -= 1;
                                            setNewPos( pos );
                                            m_curSelectBtn = 0;
                                            this->unscheduleUpdate();
                                        }
                                    }
                                });
    btn2->addTouchEventListener([this](Ref* pSender, ui::Widget::TouchEventType type)
                                {
                                    if( type == ui::Widget::TouchEventType::BEGAN )
                                    {
                                        this->scheduleUpdate(); m_fPressedTime = 0.0f;
                                        m_curSelectBtn = dynamic_cast<Node*>( pSender )->getTag();
                                    }
                                    else if (type == ui::Widget::TouchEventType::ENDED)
                                    {
                                        if (m_pNode != nullptr) {
                                            auto pos = m_pNode->getPosition();
                                            pos.y -= 1;
                                            setNewPos( pos );
                                            m_curSelectBtn = 0;
                                            this->unscheduleUpdate();
                                        }
                                    }
                                });
    btn3->addTouchEventListener([this](Ref* pSender, ui::Widget::TouchEventType type)
                                {
                                    if( type == ui::Widget::TouchEventType::BEGAN )
                                    {
                                        this->scheduleUpdate(); m_fPressedTime = 0.0f;
                                        m_curSelectBtn = dynamic_cast<Node*>( pSender )->getTag();
                                    }
                                    else if (type == ui::Widget::TouchEventType::ENDED)
                                    {
                                        if (m_pNode != nullptr) {
                                            auto pos = m_pNode->getPosition();
                                            pos.x += 1;
                                            setNewPos( pos );
                                            m_curSelectBtn = 0;
                                            this->unscheduleUpdate();
                                        }
                                    }
                                });
    btn4->addTouchEventListener([this](Ref* pSender, ui::Widget::TouchEventType type)
                                {
                                    if( type == ui::Widget::TouchEventType::BEGAN )
                                    {
                                        this->scheduleUpdate(); m_fPressedTime = 0.0f;
                                        m_curSelectBtn = dynamic_cast<Node*>( pSender )->getTag();
                                    }
                                    else if (type == ui::Widget::TouchEventType::ENDED)
                                    {
                                        if (m_pNode != nullptr) {
                                            auto pos = m_pNode->getPosition();
                                            pos.y += 1;
                                            setNewPos( pos );
                                            m_curSelectBtn = 0;
                                            this->unscheduleUpdate();
                                        }
                                    }
                                });
    
    auto btn5 = ui::Button::create(BUTTON_IMG);
    btn5->setRotation( 90 );
    btn5->setPosition(Vec2(width / 2.0f, height * 2.0f + height / 2.0f));
    addChild(btn5, 1, 5);
    
    auto btn6 = ui::Button::create(BUTTON_IMG);
    btn6->setPosition(Vec2(width + width / 2.0f,  height * 2.0f + height / 2.0f));
    addChild(btn6, 1, 6);
    
    auto btn7 = ui::Button::create(BUTTON_IMG);
    btn7->setRotation( -90 );
    btn7->setPosition(Vec2(width * 2 + width / 2.0f,  height * 2.0f + height / 2.0f));
    addChild(btn7, 1, 7);
    
    auto btn8 = ui::Button::create(BUTTON_IMG);
    btn8->setFlippedY( true );
    btn8->setPosition(Vec2(width + width / 2.0f,  height * 2.0f + height / 2.0f + height));
    addChild(btn8, 1, 8);
    
    btn5->addTouchEventListener([this](Ref* pSender, ui::Widget::TouchEventType type)
                                {
                                    if( type == ui::Widget::TouchEventType::BEGAN )
                                    {
                                        this->scheduleUpdate(); m_fPressedTime = 0.0f;
                                        m_curSelectBtn = dynamic_cast<Node*>( pSender )->getTag();
                                    }
                                    else if (type == ui::Widget::TouchEventType::ENDED)
                                    {
                                        if (m_pNode != nullptr)
                                        {
                                            auto pActor = dynamic_cast<Actor*>(m_pNode);
                                            if (pActor) {
                                                auto posAtNode = pActor->getPosAtNode();
                                                posAtNode.x -= 1;
                                                setGafNewPos( posAtNode );
                                                m_curSelectBtn = 0;
                                                this->unscheduleUpdate();
                                            }
                                        }
                                    }
                                });
    btn6->addTouchEventListener([this](Ref* pSender, ui::Widget::TouchEventType type)
                                {
                                    if( type == ui::Widget::TouchEventType::BEGAN )
                                    {
                                        this->scheduleUpdate(); m_fPressedTime = 0.0f;
                                        m_curSelectBtn = dynamic_cast<Node*>( pSender )->getTag();
                                    }
                                    else if (type == ui::Widget::TouchEventType::ENDED)
                                    {
                                        if (m_pNode != nullptr)
                                        {
                                            auto pActor = dynamic_cast<Actor*>(m_pNode);
                                            if (pActor) {
                                                auto posAtNode = pActor->getPosAtNode();
                                                posAtNode.y -= 1;
                                                setGafNewPos( posAtNode );
                                                m_curSelectBtn = 0;
                                                this->unscheduleUpdate();
                                            }
                                        }
                                    }
                                });
    btn7->addTouchEventListener([this](Ref* pSender, ui::Widget::TouchEventType type)
                                {
                                    if( type == ui::Widget::TouchEventType::BEGAN )
                                    {
                                        this->scheduleUpdate(); m_fPressedTime = 0.0f;
                                        m_curSelectBtn = dynamic_cast<Node*>( pSender )->getTag();
                                    }
                                    else if (type == ui::Widget::TouchEventType::ENDED)
                                    {
                                        if (m_pNode != nullptr)
                                        {
                                            auto pActor = dynamic_cast<Actor*>(m_pNode);
                                            if (pActor) {
                                                auto posAtNode = pActor->getPosAtNode();
                                                posAtNode.x += 1;
                                                setGafNewPos( posAtNode );
                                                m_curSelectBtn = 0;
                                                this->unscheduleUpdate();
                                            }
                                        }
                                    }
                                });
    btn8->addTouchEventListener([this](Ref* pSender, ui::Widget::TouchEventType type)
                                {
                                    if( type == ui::Widget::TouchEventType::BEGAN )
                                    {
                                        this->scheduleUpdate(); m_fPressedTime = 0.0f;
                                        m_curSelectBtn = dynamic_cast<Node*>( pSender )->getTag();
                                    }
                                    else if (type == ui::Widget::TouchEventType::ENDED)
                                    {
                                        if (m_pNode != nullptr)
                                        {
                                            auto pActor = dynamic_cast<Actor*>(m_pNode);
                                            if (pActor) {
                                                auto posAtNode = pActor->getPosAtNode();
                                                posAtNode.y += 1;
                                                setGafNewPos( posAtNode );
                                                m_curSelectBtn = 0;
                                                this->unscheduleUpdate();
                                            }
                                        }
                                    }
                                });
    
    auto btn9 = ui::Button::create(BUTTON_IMG);
    btn9->setRotation( 90 );
    btn9->setPosition(Vec2(width / 2.0f, height * 4.0f + height / 2.0f));
    addChild(btn9, 1, 9);
    
    auto btn10 = ui::Button::create(BUTTON_IMG);
    btn10->setPosition(Vec2(width + width / 2.0f,  height * 4.0f + height / 2.0f));
    addChild(btn10, 1, 10);
    
    auto btn11 = ui::Button::create(BUTTON_IMG);
    btn11->setRotation( -90 );
    btn11->setPosition(Vec2(width * 2 + width / 2.0f,  height * 4.0f + height / 2.0f));
    addChild(btn11, 1, 11);
    
    auto btn12 = ui::Button::create(BUTTON_IMG);
    btn12->setFlippedY( true );
    btn12->setPosition(Vec2(width + width / 2.0f,  height * 4.0f + height / 2.0f + height));
    addChild(btn12, 1, 12);
    
    btn9->addTouchEventListener([this](Ref* pSender, ui::Widget::TouchEventType type)
                                {
                                    if( type == ui::Widget::TouchEventType::BEGAN )
                                    {
                                        this->scheduleUpdate(); m_fPressedTime = 0.0f;
                                        m_curSelectBtn = dynamic_cast<Node*>( pSender )->getTag();
                                    }
                                    else if (type == ui::Widget::TouchEventType::ENDED)
                                    {
                                        if (m_pNode != nullptr)
                                        {
                                            auto pActor = dynamic_cast<Actor*>(m_pNode);
                                            if (pActor) {
                                                auto size = pActor->getSize();
                                                if (size.width > 0.0f) {
                                                    size.width -= 1.0f;
                                                    pActor->setSize(size);
                                                    log("new size width = %f height = %f", size.width, size.height);
                                                    m_curSelectBtn = 0;
                                                    this->unscheduleUpdate();
                                                }
                                            }
                                        }
                                    }
                                });
    btn10->addTouchEventListener([this](Ref* pSender, ui::Widget::TouchEventType type)
                                 {
                                     if( type == ui::Widget::TouchEventType::BEGAN )
                                     {
                                         this->scheduleUpdate(); m_fPressedTime = 0.0f;
                                         m_curSelectBtn = dynamic_cast<Node*>( pSender )->getTag();
                                     }
                                     else if (type == ui::Widget::TouchEventType::ENDED)
                                     {
                                         if (m_pNode != nullptr)
                                         {
                                             auto pActor = dynamic_cast<Actor*>(m_pNode);
                                             if (pActor) {
                                                 auto size = pActor->getSize();
                                                 if (size.height > 0.0f) {
                                                     size.height -= 1.0f;
                                                     pActor->setSize(size);
                                                     log("new size width = %f height = %f", size.width, size.height);
                                                     m_curSelectBtn = 0;
                                                     this->unscheduleUpdate();
                                                 }
                                             }
                                             
                                             auto pSprite = dynamic_cast<Sprite*>(m_pNode);
                                             if (pSprite) {
                                                 auto scale = pSprite->getScale();
                                                 scale -= 0.01f;
                                                 pSprite->setScale(scale);
                                                 log("new scale = %f", scale);
                                                 m_curSelectBtn = 0;
                                                 this->unscheduleUpdate();
                                             }
                                         }
                                     }
                                 });
    btn11->addTouchEventListener([this](Ref* pSender, ui::Widget::TouchEventType type)
                                 {
                                     if( type == ui::Widget::TouchEventType::BEGAN )
                                     {
                                         this->scheduleUpdate(); m_fPressedTime = 0.0f;
                                         m_curSelectBtn = dynamic_cast<Node*>( pSender )->getTag();
                                     }
                                     else if (type == ui::Widget::TouchEventType::ENDED)
                                     {
                                         if (m_pNode != nullptr)
                                         {
                                             auto pActor = dynamic_cast<Actor*>(m_pNode);
                                             if (pActor) {
                                                 auto size = pActor->getSize();
                                                 size.width += 1.0f;
                                                 pActor->setSize(size);
                                                 log("new size width = %f height = %f", size.width, size.height);
                                                 m_curSelectBtn = 0;
                                                 this->unscheduleUpdate();
                                             }
                                         }
                                     }
                                 });
    btn12->addTouchEventListener([this](Ref* pSender, ui::Widget::TouchEventType type)
                                 {
                                     if( type == ui::Widget::TouchEventType::BEGAN )
                                     {
                                         this->scheduleUpdate(); m_fPressedTime = 0.0f;
                                         m_curSelectBtn = dynamic_cast<Node*>( pSender )->getTag();
                                     }
                                     else if (type == ui::Widget::TouchEventType::ENDED)
                                     {
                                         if (m_pNode != nullptr)
                                         {
                                             auto pActor = dynamic_cast<Actor*>(m_pNode);
                                             if (pActor) {
                                                 auto size = pActor->getSize();
                                                 size.height += 1.0f;
                                                 pActor->setSize(size);
                                                 log("new size width = %f height = %f", size.width, size.height);
                                                 m_curSelectBtn = 0;
                                                 this->unscheduleUpdate();
                                             }
                                             
                                             auto pSprite = dynamic_cast<Sprite*>(m_pNode);
                                             if (pSprite) {
                                                 auto scale = pSprite->getScale();
                                                 scale += 0.01f;
                                                 pSprite->setScale(scale);
                                                 log("new scale = %f", scale);
                                                 m_curSelectBtn = 0;
                                                 this->unscheduleUpdate();
                                             }
                                         }
                                     }
                                 });
    
    // 事件派发
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    auto listenerDrag = EventListenerTouchOneByOne::create();
    listenerDrag->onTouchBegan = [this](Touch* touch, Event* event) -> bool
    {
        auto target = static_cast<Sprite*>(event->getCurrentTarget());
        auto touchPoint = target->convertTouchToNodeSpace(touch);
        Rect rect = Rect(0.0f, 0.0f, target->getContentSize().width, target->getContentSize().height);
        if (rect.containsPoint(touchPoint))
        {
            m_originalPoint = target->getPosition();
            m_touchBeganPoint = touch->getLocation();
            return true;
        }
        return false;
    };
    listenerDrag->onTouchMoved = [this](Touch* touch, Event* event)
    {
        auto target = static_cast<Sprite*>(event->getCurrentTarget());
        Vec2 touchMovePoint = touch->getLocation();
        Vec2 m_movePos;
        
        m_movePos.x = m_originalPoint.x + (touchMovePoint.x - m_touchBeganPoint.x);
        m_movePos.y = m_originalPoint.y + (touchMovePoint.y - m_touchBeganPoint.y);
        target->setPosition(m_movePos);
        this->updateArrawDirection();
    };
    listenerDrag->onTouchEnded = [this](Touch* touch, Event* event)
    {
        auto target = static_cast<Sprite*>(event->getCurrentTarget());
        auto touchPoint = m_pNode->convertTouchToNodeSpace(touch);
        Rect rect = Rect(0.0f, 0.0f,
                         m_pNode->getContentSize().width,
                         m_pNode->getContentSize().height);
        if (rect.containsPoint(touchPoint))
        {
            log("m_pNode   pos X === %f,   Y ==== %f  \n",target->getPosition().x,target->getPosition().y);
            this->updateArrawDirection();
        }
    };
    dispatcher->addEventListenerWithSceneGraphPriority(listenerDrag, m_pNode);
    
    return true;
}

void adjustPosNode::update( float dt )
{
    m_fPressedTime += dt;
    auto addValue = m_fPressedTime - 1.5f;
    if( m_fPressedTime > 1.5f )
    {
        switch ( m_curSelectBtn )
        {
            case 1:
            {
                if (m_pNode != nullptr) {
                    auto pos = m_pNode->getPosition();
                    //                    pos.x -= 1;
                    pos.x -= addValue;
                    setNewPos( pos );
                }
            }
                break;
            case 2:
            {
                if (m_pNode != nullptr) {
                    auto pos = m_pNode->getPosition();
                    //                    pos.y -= 1;
                    pos.y -= addValue;
                    setNewPos( pos );
                }
            }
                break;
            case 3:
            {
                if (m_pNode != nullptr) {
                    auto pos = m_pNode->getPosition();
                    //                    pos.x += 1;
                    pos.x += addValue;
                    setNewPos( pos );
                }
            }
                break;
            case 4:
            {
                if (m_pNode != nullptr) {
                    auto pos = m_pNode->getPosition();
                    //                    pos.y += 1;
                    pos.y += addValue;
                    setNewPos( pos );
                }
                
            }
                break;
            case 5:
            {
                if (m_pNode != nullptr)
                {
                    auto pActor = dynamic_cast<Actor*>(m_pNode);
                    if (pActor) {
                        auto posAtNode = pActor->getPosAtNode();
                        //                        posAtNode.x -= 1;
                        posAtNode.x -= addValue;
                        setGafNewPos( posAtNode );
                    }
                }
            }
                break;
            case 6:
            {
                if (m_pNode != nullptr)
                {
                    auto pActor = dynamic_cast<Actor*>(m_pNode);
                    if (pActor) {
                        auto posAtNode = pActor->getPosAtNode();
                        //                        posAtNode.y -= 1;
                        posAtNode.y -= addValue;
                        setGafNewPos( posAtNode );
                    }
                }
            }
                break;
            case 7:
            {
                if (m_pNode != nullptr)
                {
                    auto pActor = dynamic_cast<Actor*>(m_pNode);
                    if (pActor) {
                        auto posAtNode = pActor->getPosAtNode();
                        //                        posAtNode.x += 1;
                        posAtNode.x += addValue;
                        setGafNewPos( posAtNode );
                    }
                }
            }
                break;
            case 8:
            {
                if (m_pNode != nullptr)
                {
                    auto pActor = dynamic_cast<Actor*>(m_pNode);
                    if (pActor) {
                        auto posAtNode = pActor->getPosAtNode();
                        //                        posAtNode.y += 1;
                        posAtNode.y += addValue;
                        setGafNewPos( posAtNode );
                    }
                }
            }
                break;
            case 9:
            {
                if (m_pNode != nullptr)
                {
                    auto pActor = dynamic_cast<Actor*>(m_pNode);
                    if (pActor) {
                        auto size = pActor->getSize();
                        if (size.width > 0.0f) {
                            size.width -= 1.0f;
                            pActor->setSize(size);
                            log("new size width = %f height = %f", size.width, size.height);
                        }
                    }
                }
            }
                break;
            case 10:
            {
                if (m_pNode != nullptr)
                {
                    auto pActor = dynamic_cast<Actor*>(m_pNode);
                    if (pActor) {
                        auto size = pActor->getSize();
                        if (size.height > 0.0f) {
                            size.height -= 1.0f;
                            pActor->setSize(size);
                            log("new size width = %f height = %f", size.width, size.height);
                        }
                    }
                    
                    auto pSprite = dynamic_cast<Sprite*>(m_pNode);
                    if (pSprite) {
                        auto scale = pSprite->getScale();
                        scale -= 0.01f;
                        pSprite->setScale(scale);
                        log("new scale = %f", scale);
                    }
                }
            }
                break;
            case 11:
            {
                if (m_pNode != nullptr)
                {
                    auto pActor = dynamic_cast<Actor*>(m_pNode);
                    if (pActor) {
                        auto size = pActor->getSize();
                        size.width += 1.0f;
                        pActor->setSize(size);
                        log("new size width = %f height = %f", size.width, size.height);
                    }
                }
            }
                break;
            case 12:
            {
                if (m_pNode != nullptr)
                {
                    auto pActor = dynamic_cast<Actor*>(m_pNode);
                    if (pActor) {
                        auto size = pActor->getSize();
                        size.height += 1.0f;
                        pActor->setSize(size);
                        log("new size width = %f height = %f", size.width, size.height);
                    }
                    
                    auto pSprite = dynamic_cast<Sprite*>(m_pNode);
                    if (pSprite) {
                        auto scale = pSprite->getScale();
                        scale += 0.01f;
                        pSprite->setScale(scale);
                        log("new scale = %f", scale);
                    }
                }
            }
                break;
            default:
                break;
        }
    }
}

void adjustPosNode::setNewPos( const Vec2& pos )
{
    if( m_pNode )
    {
        m_pNode->setPosition( pos );
        updateArrawDirection();
        log("new pos x = %f y = %f", pos.x, pos.y);
    }
}

void adjustPosNode::setGafNewPos( const Vec2& pos )
{
    
    if( m_pNode )
    {
        auto pActor = dynamic_cast<Actor*>(m_pNode);
        if ( pActor ) {
            pActor->setPosAtNode( pos );
            updateArrawDirection();
            log("new posAtNode x = %f y = %f", pos.x, pos.y);
        }
    }
}

#define PI 3.141592653
#define dutorad(X) ((X)/180.0f*PI)
#define radtodu(X) ((X)/PI*180.0f)

void adjustPosNode::updateArrawDirection()
{
    auto pArraw = this->getChildByTag( 99 );
    if( pArraw )
    {
        auto posArraw = pArraw->getPosition();
        posArraw = this->convertToWorldSpace( posArraw );
        auto pActor = dynamic_cast<Actor*>(m_pNode);
        if( pActor )
        {
            if( pActor->getGafObj() )
            {
                auto rect = pActor->getGafObj()->getBoundingBoxForCurrentFrame();
                
                Vec2 pVec = pActor->convertToWorldSpace( Vec2(rect.getMidX(), rect.getMidY()) ) - posArraw ;
                float angle =  pVec.getNormalized().getAngle( Vec2(0.0f, -1.0f) );
                angle = radtodu( angle );
                pArraw->setRotation( angle );
                
                //            log( "pVec(%f, %f)", pVec.getNormalized().x, pVec.getNormalized().y );
                //            log( "arraw rotation:%f", angle );
            }
            else
            {
                log( "cur gaf animtion is NULL!" );
            }
        }
    }
}
