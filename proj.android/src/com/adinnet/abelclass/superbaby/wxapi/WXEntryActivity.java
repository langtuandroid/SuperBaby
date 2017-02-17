package com.adinnet.abelclass.superbaby.wxapi;

import com.tencent.mm.sdk.modelbase.BaseReq;
import com.tencent.mm.sdk.modelbase.BaseResp;
import com.tencent.mm.sdk.openapi.IWXAPIEventHandler;
import com.tencent.mm.sdk.openapi.IWXAPI;
import com.tencent.mm.sdk.openapi.WXAPIFactory;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.util.Log;

import org.cocos2dx.cpp.AppActivity;

//import android.widget.Toast;

public class WXEntryActivity extends Activity implements IWXAPIEventHandler 
{
	private IWXAPI mWxApi;
	
    @Override
    protected void onCreate(final Bundle savedInstanceState) 
    {
    	super.onCreate(savedInstanceState);
    	
//		Log.i("org.cocos2dx.cpp", "WXEntryActivity onCreate called, TaskId = " + this.getTaskId());
    	
	    // 注册微信 app id
    	mWxApi = WXAPIFactory.createWXAPI(this, AppActivity.WX_APP_ID , false);
    	mWxApi.registerApp(AppActivity.WX_APP_ID);
    	mWxApi.handleIntent(getIntent(), this);
    }
    
	@Override
	protected void onNewIntent(Intent intent) 
	{
		super.onNewIntent(intent);
		setIntent(intent);
		mWxApi.handleIntent(intent, this);
	}
    
	@Override
	protected void onDestroy() 
	{
		super.onDestroy();
		if (mWxApi != null) 
		{
			mWxApi.unregisterApp();
		}
	}
	
	@Override
	public void onReq(BaseReq req) 
	{
		Log.i("org.cocos2dx.cpp", "wx onReq");
	}

	@Override
	public void onResp(BaseResp resp) 
	{
		switch (resp.errCode) 
		{
        case BaseResp.ErrCode.ERR_OK:  
            // 分享成功
        	break;  
        case BaseResp.ErrCode.ERR_USER_CANCEL:  
            // 分享取消  
            break;  
        case BaseResp.ErrCode.ERR_AUTH_DENIED:  
            // 分享拒绝  
            break;  
        default:
        	break;
        }
    	this.startActivity(new Intent(WXEntryActivity.this, org.cocos2dx.cpp.AppActivity.class));
    	this.finish();
	}
}
