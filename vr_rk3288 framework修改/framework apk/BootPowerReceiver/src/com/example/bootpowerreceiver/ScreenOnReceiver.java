package com.example.bootpowerreceiver;

import java.io.BufferedReader;
import java.io.FileReader;
import java.io.IOException;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.os.PowerManager;
import android.os.SystemClock;
import android.util.Log;

public class ScreenOnReceiver extends BroadcastReceiver{
	String path = "/sys/devices/ff100000.adc/key.30/get_gsensor_value";//节点文件路径
	private PowerManager pm = null;// 电源管理对象
	@Override
	public void onReceive(Context context, Intent intent) {
		String action = intent.getAction();
		if (Intent.ACTION_SCREEN_ON.equals(intent.getAction())) { 
			Log.i("ScreenOnReceiver", "ScreenOnReceiver = "+Intent.ACTION_SCREEN_ON);
			   screenOnDone(context);
		}  
		
	}
	public void screenOnDone(Context context){
		if(null == pm){
			pm = (PowerManager) context.getSystemService(Context.POWER_SERVICE);
		}
		String str = mygetString(path);
		Log.i("ScreenOnReceiver", "screenOnDone str = "+str);
		if("1".equals(str)){
			if(pm.isScreenOn()){
				new Thread(){
					@Override
					public void run() {
						// TODO Auto-generated method stub
						super.run();
						try {
							Thread.sleep(1000);
						} catch (InterruptedException e) {
							// TODO Auto-generated catch block
							e.printStackTrace();
						}
						pm.goToSleep(SystemClock.uptimeMillis());
					}
					
				}.start();
			}
		}
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
}
