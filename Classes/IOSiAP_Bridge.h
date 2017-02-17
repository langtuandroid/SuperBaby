//
//  IOSiAP_Bridge.h
//
//  Created by yangjie on 2015-05-25.
//
//

#ifndef __AbelLLExplorer__IOSiAP_Bridge__
#define __AbelLLExplorer__IOSiAP_Bridge__

#include "IOSiAP.h"
#include <vector>
#include <functional>

class IOSiAP_Bridge : public IOSiAPDelegate
{
public:
    virtual ~IOSiAP_Bridge();

private:
    IOSiAP_Bridge();
    IOSiAP_Bridge(const IOSiAP_Bridge&);
    IOSiAP_Bridge& operator=(const IOSiAP_Bridge&);
    
public:
    static IOSiAP_Bridge* getInstance();
    
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
