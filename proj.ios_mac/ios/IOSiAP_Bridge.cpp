//
//  IOSiAP_Bridge.cpp
//  AbelLLExplorer
//
//  Created by yangjie on 2015-05-25.
//
//

#include "IOSiAP_Bridge.h"
#include "cocos2d.h"

// 单例
static IOSiAP_Bridge *pIOSiAP_Bridge = nullptr;

/*
 *  constructor
 */
IOSiAP_Bridge::IOSiAP_Bridge()
{
    m_pIap = new IOSiAP();
    m_pIap->delegate = this;
    
    m_products.push_back("adinnet.poor.jzl.smxq");
//    m_products.push_back("adinnet.poor.jzl.hlhy");
//    m_products.push_back("adinnet.poor.jzl.wmgg");
}

/*
 *  destructor
 */
IOSiAP_Bridge::~IOSiAP_Bridge()
{
    CC_SAFE_DELETE(m_pIap);
}

/*
 *  获取单例
 */
IOSiAP_Bridge* IOSiAP_Bridge::getInstance()
{
    if (nullptr == pIOSiAP_Bridge)
    {
        pIOSiAP_Bridge = new IOSiAP_Bridge();
    }
    return pIOSiAP_Bridge;
}

/*
 *  销毁单例
 */
void IOSiAP_Bridge::destroryInstance()
{
    CC_SAFE_DELETE(pIOSiAP_Bridge);
}

/**/
bool IOSiAP_Bridge::checkNetwork()
{
    return m_pIap->checkNetWork();
}

/*
 *  id
 *  产品id
 *  2:神秘星球
 *  3:欢乐海洋
 *  4:文明古国
 */
void IOSiAP_Bridge::requestProducts(int id)
{
    m_onRestoreReturn = nullptr;
    m_productID = id;

	// 把需要付费的道具的所有product id都放到容器里面传进去
    m_pIap->requestProducts(m_products);
}

//
void IOSiAP_Bridge::restore()
{
    m_onBuyReturn = nullptr;
    m_productID = 2;
    m_pIap->restore();
}

void IOSiAP_Bridge::onRequestProductsFinish(void)
{
    cocos2d::log("onRequestProductsFinish");
    std::string identifier = "";
    switch (m_productID)
    {
        case 2:
            identifier = "adinnet.poor.jzl.smxq";
            break;
        case 3:
            identifier = "adinnet.poor.jzl.hlhy";
            break;
        case 4:
            identifier = "adinnet.poor.jzl.wmgg";
            break;
        default:
            break;
    }

    // 必须在onRequestProductsFinish后才能去请求iAP产品数据。
    IOSProduct *product = m_pIap->iOSProductByIdentifier(identifier);
    m_pIap->paymentWithProduct(product, 1);
}

//
void IOSiAP_Bridge::onRequestProductsError(int code)
{
    cocos2d::log("付款失败 错误码 = %d", code);
    if (m_onBuyReturn != nullptr)
    {
        m_onBuyReturn(false);
    }
}

//
void IOSiAP_Bridge::onPaymentEvent(std::string &identifier, IOSiAPPaymentEvent event, int quantity)
{
    bool isBuySuccessed = false;
    if (event == IOSIAP_PAYMENT_PURCHAED || event == IOSIAP_PAYMENT_RESTORED)
    {
        isBuySuccessed = true;
        cocos2d::log("付款或恢复成功");
        switch (m_productID)
        {
            case 2:
                cocos2d::UserDefault::getInstance()->setBoolForKey("adinnet.poor.jzl.smxq", true);
                cocos2d::UserDefault::getInstance()->flush();
                break;
            case 3:
                cocos2d::UserDefault::getInstance()->setBoolForKey("adinnet.poor.jzl.hlhy", true);
                cocos2d::UserDefault::getInstance()->flush();
                break;
            case 4:
                cocos2d::UserDefault::getInstance()->setBoolForKey("adinnet.poor.jzl.wmgg", true);
                cocos2d::UserDefault::getInstance()->flush();
                break;
            default:
                break;
        }        
    }
    if (m_onBuyReturn != nullptr)
    {
        m_onBuyReturn(isBuySuccessed);
    }
    if (m_onRestoreReturn) {
        m_onRestoreReturn(isBuySuccessed);
    }
}

//
void IOSiAP_Bridge::onRestoreFail()
{
    if (m_onRestoreReturn != nullptr)
    {
        m_onRestoreReturn(false);
    }
}

//
void IOSiAP_Bridge::onRestoreSuccess()
{
    if (m_onRestoreReturn != nullptr)
    {
        m_onRestoreReturn(true);
    }
}
