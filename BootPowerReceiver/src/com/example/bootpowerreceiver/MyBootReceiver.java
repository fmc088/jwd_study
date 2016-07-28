package com.example.bootpowerreceiver;

import java.io.BufferedReader;
import java.io.FileReader;
import java.io.IOException;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.os.PowerManager;
import android.util.Log;
import android.view.WindowManager;

public class MyBootReceiver extends BroadcastReceiver {
	private final static String TAG = "MyBootReceiver";
	String path = "/sys/devices/ff100000.adc/key.32/get_gsensor_value";
	private PowerManager localPowerManager = null;//电源管理对象
	private WindowManager wManager;
	@Override
	public void onReceive(Context arg0, Intent arg1) {
		// TODO Auto-generated method stub
		String action = arg1.getAction();
		
		if (Intent.ACTION_BOOT_COMPLETED.equals(action)) { 
			arg0.startService(new Intent(arg0,PowerSensorService.class)); 
			//wManager = 
			//WindowManager.LayoutParams lp = arg0. 
			//localPowerManager = (PowerManager)arg0.getSystemService(Context.POWER_SERVICE);
			//new PowerThread().start();
		}
	}
	
	public String getString (String path){
		String prop = "fail";
		try {
			BufferedReader reader = new BufferedReader(new FileReader(path));
			prop = reader.readLine();
			reader.close();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		Log.i(TAG, "----prop -------="+prop+"----");
		return prop;
	}
//	public static void setBrightness(Activity activity, int brightness) {     
//		     
//	    WindowManager.LayoutParams lp = activity.getWindow().getAttributes();     
//	     lp.screenBrightness = Float.valueOf(brightness) * (1f / 255f);    
//	     Log.d("lxy", "set  lp.screenBrightness == " + lp.screenBrightness);  
//	  
//	     activity.getWindow().setAttributes(lp);   
//	     } 
	public class PowerThread extends Thread{

		@Override
		public void run() {
			// TODO Auto-generated method stub
			super.run();
			while(true){
				Log.i(TAG, "---PowerThread ---start----");
				try {
					Thread.sleep(1000);
				} catch (InterruptedException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
				try {
					String str = getString(path);
					Log.i(TAG, "String getString ="+str+"---");
					if("0".equals(str.trim())){ //读取0时亮屏
						Log.i(TAG, "---PowerThread isScreenOn ="+localPowerManager.isScreenOn());
						if(!localPowerManager.isScreenOn()){
							try {
		                		//Log.d(TAG,"input keyevent KEYCODE_POWER start");
								Runtime.getRuntime().exec("input keyevent KEYCODE_POWER");
								//Log.d(TAG,"input keyevent KEYCODE_POWER end");
							} catch (IOException e) {
								//Log.d(TAG,"input keyevent KEYCODE_POWER error"+e.getMessage());
								// TODO Auto-generated catch block
								e.printStackTrace();
							}
						}
					}else if("fail".equals(str.trim())){
						Log.i(TAG, "getString fail");
					}else{//读取到1，屏幕灭
						Log.i(TAG, "---PowerThread isScreenOn ="+localPowerManager.isScreenOn());
                        if(localPowerManager.isScreenOn()){
                        	try {
                        		//Log.d(TAG,"input keyevent KEYCODE_POWER start");
        						Runtime.getRuntime().exec("input keyevent KEYCODE_POWER");
        						//Log.d(TAG,"input keyevent KEYCODE_POWER end");
        					} catch (IOException e) {
        						
        						//Log.d(TAG,"input keyevent KEYCODE_POWER error"+e.getMessage());
        						// TODO Auto-generated catch block
        						e.printStackTrace();
        					}
						}
					}
				} catch (Exception e) {
					Log.e(TAG,"BootReceiver :"+ e.getMessage());
					e.printStackTrace();
				}
			}
		}
		
	}

}
