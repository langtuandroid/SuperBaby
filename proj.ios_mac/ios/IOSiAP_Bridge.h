//
//  IOSiAP_Bridge.h
//
//  yangjie
//
//  Created by 白白 on 2015-05-25.
//
//

#ifndef __AbelLLExplorer__IOSiAP_Bridge__
#define __AbelLLExplorer__IOSiAP_Bridge__

#import "IOSiAP.h"
#include <vector>

class IOSiAP_Bridge : public IOSiAPDelegate
{
private:
    IOSiAP_Bridge();
public:
    ~IOSiAP_Bridge();

public:
    static IOSiAP_Bridge* getInstance();
    static void destroryInstance();
    
public:
    /**/
    bool checkNetwork();
    
    /**/
    void requestProducts(int);
    /**/
    inline void setOnBuyReturn(const std::function<void (bool)>& cb) {m_onBuyReturn = cb;};
    /**/
    void restore();
    /**/
    inline void setOnRestoreReturn(const std::function<void (bool)>& cb) {m_onRestoreReturn = cb;};
    
    virtual void onRequestProductsFinish(void);
    virtual void onRequestProductsError(int code);
    virtual void onPaymentEvent(std::string &identifier, IOSiAPPaymentEvent event, int quantity);
    // for restore
    virtual void onRestoreFail();
    virtual void onRestoreSuccess();
    
private:
    IOSiAP *m_pIap = nullptr;
    int m_productID = -1;
    
    std::vector<std::string> m_products;
    
    std::function<void (bool)> m_onBuyReturn = nullptr;
    /* 恢复商品回调 */
    std::function<void (bool)> m_onRestoreReturn = nullptr;
};
#endif /* defined(__AbelLLExplorer__IOSiAP_Bridge__) */
