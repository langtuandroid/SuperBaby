//
//  IOSiAP.h
//
//  Created by yangjie on 2015-05-25.
//
//

#ifndef __AbelLLExplorer__IOSiAP__
#define __AbelLLExplorer__IOSiAP__

#include <iostream>
#include <vector>

class IOSProduct
{
public:
    std::string productIdentifier;
    std::string localizedTitle;
    std::string localizedDescription;
    std::string localizedPrice;// has be localed, just display it on UI.
    bool isValid;
    int index;//internal use : index of skProducts
};

typedef enum
{
    IOSIAP_PAYMENT_PURCHASING,// just notify, UI do nothing
    IOSIAP_PAYMENT_PURCHAED,// need unlock App Functionality
    IOSIAP_PAYMENT_FAILED,// remove waiting on UI, tall user payment was failed
    IOSIAP_PAYMENT_RESTORED,// need unlock App Functionality, consumble payment No need to care about this.
    IOSIAP_PAYMENT_REMOVED,// remove waiting on UI
} IOSiAPPaymentEvent;

class IOSiAPDelegate
{
public:
    virtual ~IOSiAPDelegate() {}
    // for requestProduct
    virtual void onRequestProductsFinish(void) = 0;
    virtual void onRequestProductsError(int code) = 0;
    // for payment
    virtual void onPaymentEvent(std::string &identifier, IOSiAPPaymentEvent event, int quantity) = 0;
    // for restore
    virtual void onRestoreSuccess() = 0;
    virtual void onRestoreFail() = 0;
};

class IOSiAP
{
public:
    IOSiAP();
    ~IOSiAP();
    
public:
    bool checkNetWork();
    
    void requestProducts(std::vector <std::string> &productIdentifiers);
    IOSProduct *iOSProductByIdentifier(std::string &identifier);
    void paymentWithProduct(IOSProduct *iosProduct, int quantity = 1);
    
    void restore();
    
    IOSiAPDelegate *delegate = nullptr;
    // ===  internal use for object-c class ===
    void *skProducts;// object-c SKProduct
    void *skTransactionObserver;// object-c TransactionObserver
    std::vector<IOSProduct*> m_iOSProducts;
};

#endif /* defined(__AbelLLExplorer__IOSiAP__) */
