/****************************************************************************
Copyright (c) 2008-2010 Ricardo Quesada
Copyright (c) 2010-2012 cocos2d-x.org
Copyright (c) 2011      Zynga Inc.
Copyright (c) 2013-2014 Chukong Technologies Inc.
 
http://www.cocos2d-x.org

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
****************************************************************************/
package org.cocos2dx.cpp;

import java.io.File;

import org.cocos2dx.lib.Cocos2dxActivity;

import android.content.Context;
import android.content.Intent;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.net.ConnectivityManager;
import android.net.NetworkInfo;
import android.net.Uri;
import android.os.Bundle;
import android.util.Log;
//import android.util.Log;
import android.widget.Toast;

import com.adinnet.abelclass.superbaby.R;
import com.tencent.mm.sdk.modelmsg.SendMessageToWX;
import com.tencent.mm.sdk.modelmsg.WXMediaMessage;
import com.tencent.mm.sdk.modelmsg.WXWebpageObject;
import com.tencent.mm.sdk.openapi.IWXAPI;
import com.tencent.mm.sdk.openapi.WXAPIFactory;
import com.umeng.mobclickcpp.MobClickCppHelper;

public class AppActivity extends Cocos2dxActivity 
{
	public static String APP_TITLE = "超能宝贝";

    public static final String WX_APP_ID = "wx06c959d1ad45e8e2";
	private static IWXAPI mWxApi = null;
	
	public static final String HOME_PAGE = "http://www.Abelclass.com";
	
	private static AppActivity instance;

	@Override
	protected void onCreate(Bundle savedInstanceState) 
	{
		super.onCreate(savedInstanceState);
        
		Log.i("org.cocos2dx.cpp", "AppActivity onCreate called, TaskId = " + this.getTaskId());
		
        // umeng
        MobClickCppHelper.init(this);
		
		instance = this;
		
	    // 注册微信 app id
		mWxApi = WXAPIFactory.createWXAPI(this, WX_APP_ID, false);
		mWxApi.registerApp(WX_APP_ID);
	}
    
	@Override
	protected void onNewIntent(Intent intent) 
	{
		super.onNewIntent(intent);
		setIntent(intent);
	}
	
    @Override
    protected void onStart() 
    {
        super.onStart();
    }  
	
	@Override
	protected void onRestart() 
	{
		super.onRestart();
	}
	
	@Override
	protected void onResume() 
	{
		super.onResume();
	}
	
    @Override  
    protected void onPause() 
    {  
        super.onPause();  
    }  
    
    @Override
    protected void onStop() 
    {
        super.onStop();  
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
	
    public static void sendMsgToWxTimeLine(String shareMessage) 
    {
//    	System.out.println("sendMsgToTimeLine");
    	if (mWxApi.openWXApp())
    	{
        	if (mWxApi.getWXAppSupportAPI() >= 0x21020001)
        	{
        		WXWebpageObject webpage = new WXWebpageObject();
                webpage.webpageUrl = HOME_PAGE;
                WXMediaMessage msg = new WXMediaMessage(webpage);
                msg.title = shareMessage;
                msg.description = shareMessage;
                
                Bitmap thumb = BitmapFactory.decodeResource(instance.getResources(), R.drawable.icon);
                msg.thumbData = Util.bmpToByteArray(thumb, true);
                 
                SendMessageToWX.Req req = new SendMessageToWX.Req();
                req.transaction = buildTransaction("webpage");
                req.message = msg;
                req.scene = SendMessageToWX.Req.WXSceneTimeline;
                mWxApi.sendReq(req);
        	} else {
        		Toast.makeText(instance, "微信版本过低", Toast.LENGTH_SHORT).show();
        	}
    	} else {
    		 Toast.makeText(instance, "未安装微信", Toast.LENGTH_SHORT).show();
    	}
    }
    
    public static void sendMsgToWxSession(String shareMessage) 
    {
//    	System.out.println("sendMsgToSession");
    	if (mWxApi.openWXApp())
    	{
        	if (mWxApi.getWXAppSupportAPI() >= 0x21020001)
        	{
        		WXWebpageObject webpage = new WXWebpageObject();
                webpage.webpageUrl = HOME_PAGE;
                WXMediaMessage msg = new WXMediaMessage(webpage);
                msg.title = shareMessage;
                msg.description = shareMessage;
                
                Bitmap thumb = BitmapFactory.decodeResource(instance.getResources(), R.drawable.icon);
                msg.thumbData = Util.bmpToByteArray(thumb, true);
                 
                SendMessageToWX.Req req = new SendMessageToWX.Req();
                req.transaction = buildTransaction("webpage");
                req.message = msg;
                req.scene = SendMessageToWX.Req.WXSceneSession;
                mWxApi.sendReq(req);
        	} else {
        		Toast.makeText(instance, "微信版本过低", Toast.LENGTH_SHORT).show();
        	}
    	} else {
    		 Toast.makeText(instance, "未安装微信", Toast.LENGTH_SHORT).show();
    	}
    }
    
    public void sendMsgToWeibo(String shareMsg) 
    {
    			Bundle bundle = new Bundle();
    			bundle.putString("weiboShareMsg", shareMsg);
    			Intent intent = new Intent(AppActivity.this, com.adinnet.abelclass.superbaby.wbapi.WBShareActivity.class);
    			intent.putExtras(bundle);
    			this.startActivity(intent);
    }
    
	private static String buildTransaction(final String type) 
	{
		return (type == null) ? String.valueOf(System.currentTimeMillis()) : type + System.currentTimeMillis();
	}		
	
	public static boolean isNetworkAvailable() 
	{ 
		boolean isAvailable = false;
		ConnectivityManager cm = (ConnectivityManager)instance.getSystemService(Context.CONNECTIVITY_SERVICE); 
		if (cm != null) {
			isAvailable = cm.getActiveNetworkInfo().isAvailable();
		}  
		return isAvailable;
    }

	public static boolean isWifiEnable() 
	{ 
        ConnectivityManager connectivityManager = (ConnectivityManager)instance.getSystemService(Context.CONNECTIVITY_SERVICE);  
        boolean isWifiDataEnable = false;  
        isWifiDataEnable = connectivityManager.getNetworkInfo(ConnectivityManager.TYPE_WIFI).isConnectedOrConnecting();  
        return isWifiDataEnable;  		
    }

	public static Object cppCall_logsth()  
	{
	    return instance;
	}
	
	public void updateVersion(String apkFile)  
	{  
//		Log.i("cocos2d-x debug info", apkFile);
	    Intent intent = new Intent(Intent.ACTION_VIEW);
	    intent.setDataAndType(Uri.fromFile(new File(apkFile)),
	            "application/vnd.android.package-archive");  
	    instance.startActivity(intent);  
	} 
	
	public boolean isConnectionAvailable()
	{
		boolean isConn = false;
  		ConnectivityManager cwjManager = (ConnectivityManager)getSystemService(Context.CONNECTIVITY_SERVICE);
		NetworkInfo networkInfo = cwjManager.getActiveNetworkInfo(); 
        if (networkInfo != null) 
        {
            isConn = networkInfo.isAvailable(); 
        }
        return isConn;
	}

    static 
    {
        MobClickCppHelper.loadLibrary();
    }	
}
