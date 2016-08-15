package com.example.bootoperationreceiver;

import java.io.BufferedReader;
import java.io.FileReader;
import java.io.IOException;

import android.app.AlarmManager;
import android.app.PendingIntent;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.util.Log;

public class BootReceiver extends BroadcastReceiver{
	private final static String ACTION2 = "android.settings.remote.alarm";
	public final static String TAG = "BootReceiver";
	String path = "/sys/devices/ff100000.adc/key.30/get_gsensor_value";//节点文件路径
	@Override
	public void onReceive(Context context, Intent intent) {
		String action = intent.getAction();
		if (Intent.ACTION_BOOT_COMPLETED.equals(action)) {
			Log.i(TAG, "------BootReceiver KEYCODE_POWER--------");
			String str = mygetString(path);
			if("1".equals(str)){
				try {                                            
		            Runtime.getRuntime().exec("input keyevent KEYCODE_POWER");                                         
		            Log.i(TAG, "------BootReceiver KEYCODE_POWER--------");
		                          } catch (IOException e) {
		               Log.e(TAG,"BootReceiver scheduleStartProfilesLocked "+e.getMessage());
		                          e.printStackTrace();
		            }
			} 
		}else if(ACTION2.equals(action)){
			Log.i(TAG, "remote ------");
//			showJudgment(context);
		}else if(Intent.ACTION_SHUTDOWN.equals(action)){
			Intent intent2 = new Intent(ACTION2); 
			PendingIntent pi=PendingIntent.getBroadcast(context, 0, intent,0); 
			AlarmManager alarm=(AlarmManager)context.getSystemService(context.ALARM_SERVICE); 
			alarm.cancel(pi); }
			
	}
	
	public void wakeUpToSleep(){
		new Thread(){
			@Override
			public void run() {
				// TODO Auto-generated method stub
				super.run();
				
				try {
					Thread.sleep(1000);
                  Log.i(TAG, "---------ACTION_BOOT_COMPLETED---start--");
                                        Runtime.getRuntime().exec("input keyevent KEYCODE_POWER");
				                                         Log.i(TAG, "------BootReceiver ACTION_BOOT_COMPLETED--------");
				                                 } catch (IOException | InterruptedException e) {
				                                        
				                                       e.printStackTrace();
				                               }
					           
			}
		}.start();
	}
	
	public String mygetString (String path){
		String prop = "fail";
		try {
			BufferedReader reader = new BufferedReader(new FileReader(path));
			prop = reader.readLine();
			reader.close();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		return prop;
	}
//	public void showJudgment(Context context){
//		String path = "/sys/devices/ff100000.adc/key.30/get_gsensor_value";
//		String str = mygetString(path);
//		PowerManager pm = (PowerManager) context.getSystemService(Context.POWER_SERVICE);
//		Log.i(TAG, "curren data = "+str);
//		if("0".equals(str.trim())){ //读取0时亮屏
//			if(!pm.isScreenOn()){
//				if(null != pm){
//					pm.wakeUp(SystemClock.uptimeMillis());
//				}
//			}
//		}else if("fail".equals(str.trim())){//读取节点异常
//			Log.i(TAG, "----mygetString IOException---");
//		}else if("1".equals(str.trim())){//读取到1时屏幕灭屏，
//			if(pm.isScreenOn()){
//	            if(null != pm){
//	            	pm.goToSleep(SystemClock.uptimeMillis(),
//                            PowerManager.GO_TO_SLEEP_REASON_POWER_BUTTON, 0);
//	            	pm.goToSleep(SystemClock.uptimeMillis());
//	            }
//			}
//		}else{
//			Log.i(TAG, "---PowerThread get the error data " +str);
//		}
//	}

}
