//
//  IOSiAP.mm
//  AbelLLExplorer
//
//  Created by yangjie on 2015-05-25.
//
//
#import <Foundation/Foundation.h>
#import "Reachability.h"
#import <StoreKit/StoreKit.h>

#import "IOSiAP.h"

@interface iAPProductsRequestDelegate : NSObject<SKProductsRequestDelegate>
@property (nonatomic, assign) IOSiAP *iosiap;
@end

@implementation iAPProductsRequestDelegate

- (void)productsRequest:(SKProductsRequest *)request
     didReceiveResponse:(SKProductsResponse *)response
{
    // release old
    if (_iosiap->skProducts)
    {
        [(NSArray *)(_iosiap->skProducts) release];
    }
    // record new product
    _iosiap->skProducts = [response.products retain];
    
    for (int index = 0; index < [response.products count]; index++)
    {
        SKProduct *skProduct = [response.products objectAtIndex:index];
        
        // check is valid
        bool isValid = true;
        for (NSString *invalidIdentifier in response.invalidProductIdentifiers)
        {
            NSLog(@"invalidIdentifier:%@", invalidIdentifier);
            if ([skProduct.productIdentifier isEqualToString:invalidIdentifier])
            {
                isValid = false;
                break;
            }
        }
        
        IOSProduct *iosProduct = new IOSProduct;
        iosProduct->productIdentifier = std::string([skProduct.productIdentifier UTF8String]);
        if (skProduct.localizedTitle != nil)
        {
            iosProduct->localizedTitle = std::string([skProduct.localizedTitle UTF8String]);
        } else {
            iosProduct->localizedTitle = "localizedTitle";
        }
        if (skProduct.localizedDescription != nil)
        {
            iosProduct->localizedDescription = std::string([skProduct.localizedDescription UTF8String]);
        } else {
            iosProduct->localizedDescription = "localizedDescription";
        }
        
        // locale price to string
        NSNumberFormatter *formatter = [[NSNumberFormatter alloc] init];
        [formatter setFormatterBehavior:NSNumberFormatterBehavior10_4];
        [formatter setNumberStyle:NSNumberFormatterCurrencyStyle];
        [formatter setLocale:skProduct.priceLocale];
        NSString *priceStr = [formatter stringFromNumber:skProduct.price];
        [formatter release];
        iosProduct->localizedPrice = std::string([priceStr UTF8String]);
        
        iosProduct->index = index;
        iosProduct->isValid = isValid;
        
        //
        _iosiap->m_iOSProducts.push_back(iosProduct);
    } /*for*/
}

- (void)requestDidFinish:(SKRequest *)request
{
    _iosiap->delegate->onRequestProductsFinish();
    [request.delegate release];
    [request release];
}

- (void)request:(SKRequest *)request didFailWithError:(NSError *)error
{
    NSLog(@"%@", error);
    _iosiap->delegate->onRequestProductsError([error code]);
}

@end

@interface iAPTransactionObserver : NSObject<SKPaymentTransactionObserver>
@property (nonatomic, assign) IOSiAP *iosiap;
@end

@implementation iAPTransactionObserver

- (void)paymentQueue:(SKPaymentQueue *)queue updatedTransactions:(NSArray *)transactions
{
    for (SKPaymentTransaction *transaction in transactions) {
        std::string identifier([transaction.payment.productIdentifier UTF8String]);
        IOSiAPPaymentEvent event;
        
        switch (transaction.transactionState) {
            case SKPaymentTransactionStatePurchasing:
                event = IOSIAP_PAYMENT_PURCHASING;
                return;
            case SKPaymentTransactionStatePurchased:
                event = IOSIAP_PAYMENT_PURCHAED;
                break;
            case SKPaymentTransactionStateFailed:
                event = IOSIAP_PAYMENT_FAILED;
                NSLog(@"==ios payment error:%@", transaction.error);
                break;
            case SKPaymentTransactionStateRestored:
                NSLog(@"restore");
                // NOTE: consumble payment is NOT restorable
//                [self restoreTransaction:transaction];
                event = IOSIAP_PAYMENT_RESTORED;
                break;
        }
        
        _iosiap->delegate->onPaymentEvent(identifier, event, transaction.payment.quantity);
        if (event != IOSIAP_PAYMENT_PURCHASING) {
            [[SKPaymentQueue defaultQueue] finishTransaction: transaction];
        }
    }
}

- (void)paymentQueue:(SKPaymentQueue *)queue removedTransactions:(NSArray *)transactions
{
    for (SKPaymentTransaction *transaction in transactions) {
        std::string identifier([transaction.payment.productIdentifier UTF8String]);
        _iosiap->delegate->onPaymentEvent(identifier, IOSIAP_PAYMENT_REMOVED, transaction.payment.quantity);
    }
}

//
- (void)paymentQueue:(SKPaymentQueue *)queue restoreCompletedTransactionsFailedWithError:(NSError *)error
{
    NSLog(@"-------restoreCompletedTransactionsFailedWithError----");
    _iosiap->delegate->onRestoreFail();
}

// Sent when all transactions from the user's purchase history have successfully been added back to the queue.
- (void)paymentQueueRestoreCompletedTransactionsFinished:(SKPaymentQueue *)queue
{
    NSLog(@"-------paymentQueueRestoreCompletedTransactionsFinished----");
    if (0 >= [queue.transactions count])
    {
        NSLog(@"没有可以恢复的交易");
        _iosiap->delegate->onRestoreFail();
    }
}

@end

IOSiAP::IOSiAP():
skProducts(nullptr),
delegate(nullptr)
{
    skTransactionObserver = [[iAPTransactionObserver alloc] init];
    ((iAPTransactionObserver *)skTransactionObserver).iosiap = this;
    [[SKPaymentQueue defaultQueue] addTransactionObserver:(iAPTransactionObserver *)skTransactionObserver];
}

IOSiAP::~IOSiAP()
{
    if (skProducts) {
        [(NSArray *)(skProducts) release];
    }
    
    std::vector <IOSProduct *>::iterator iterator;
    for (iterator = m_iOSProducts.begin(); iterator != m_iOSProducts.end(); iterator++)
    {
        IOSProduct *iosProduct = *iterator;
        delete iosProduct;
    } /*for*/
    
    [[SKPaymentQueue defaultQueue] removeTransactionObserver:(iAPTransactionObserver *)skTransactionObserver];
    [(iAPTransactionObserver *)skTransactionObserver release];
}

IOSProduct *IOSiAP::iOSProductByIdentifier(std::string &identifier)
{
    std::vector <IOSProduct *>::iterator iterator;
    for (iterator = m_iOSProducts.begin(); iterator != m_iOSProducts.end(); iterator++)
    {
        IOSProduct *iosProduct = *iterator;
        if (iosProduct->productIdentifier == identifier)
        {
            return iosProduct;
        }
    }
    return nullptr;
}

//
void IOSiAP::requestProducts(std::vector <std::string> &productIdentifiers)
{
    NSMutableSet *set = [NSMutableSet setWithCapacity:productIdentifiers.size()];
    std::vector <std::string>::iterator iterator;
    for (iterator = productIdentifiers.begin(); iterator != productIdentifiers.end(); iterator++)
    {
        [set addObject:[NSString stringWithUTF8String:(*iterator).c_str()]];
    }
    SKProductsRequest *productsRequest = [[SKProductsRequest alloc] initWithProductIdentifiers:set];
    iAPProductsRequestDelegate *delegate = [[iAPProductsRequestDelegate alloc] init];
    delegate.iosiap = this;
    productsRequest.delegate = delegate;
    [productsRequest start];
}

/*
 解决方法很简单，增加一个Restore按钮，并调用[[SKPaymentQueue defaultQueue] restoreCompletedTransactions]，接下来的流程是
 
 1，弹出对话框输入帐号信息
 2，如果点击“取消”，调用方法
 -(void)paymentQueue:(SKPaymentQueue *)queue restoreCompletedTransactionsFailedWithError:(NSError *)error
 如果点击“确定”，开始请求数据
 3,返回数据后，回调
 -(void)paymentQueue:(SKPaymentQueue *)queue updatedTransactions:(NSArray *)transactions
 4,判断交易队列中交易的状态，在状态为“SKPaymentTransactionStateRestored”的switch分支下调用恢复处理方法
 5,恢复完成后回调-(void)completeTransaction:(SKPaymentTransaction *)transaction
 */
//
void IOSiAP::restore()
{
    [[SKPaymentQueue defaultQueue] restoreCompletedTransactions];
}

/*
 *  发起付款请求
 *  第一个参数是由iOSProductByIdentifier获取的IOSProduct实例，第二个参数是购买数量
 */
void IOSiAP::paymentWithProduct(IOSProduct *iosProduct, int quantity)
{
    SKProduct *skProduct = [(NSArray*)(skProducts) objectAtIndex:iosProduct->index];
    SKMutablePayment *payment = [SKMutablePayment paymentWithProduct:skProduct];
    payment.quantity = quantity;
    
    [[SKPaymentQueue defaultQueue] addPayment:payment];
}

//
bool IOSiAP::checkNetWork()
{
    Reachability* reachability = [Reachability reachabilityWithHostName:@"www.apple.com"];
    NetworkStatus status = [reachability currentReachabilityStatus];
    if (NotReachable != status) {
        return true;
    }
    return false;
}
