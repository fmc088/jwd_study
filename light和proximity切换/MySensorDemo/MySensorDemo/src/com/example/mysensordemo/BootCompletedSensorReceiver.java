package com.example.mysensordemo;

import android.app.AlarmManager;
import android.app.PendingIntent;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.os.SystemClock;
import android.util.Log;

public class BootCompletedSensorReceiver extends BroadcastReceiver{
	private final static String TAG = "BootCompletedSensorReceiver";
	private final String ACTION_BOOT = "android.intent.action.BOOT_COMPLETED";
	private final String ACTION = "com.destroy.restart.service";
	private final String SERVICE_ACTION = "com.LightSensorService";
	@Override
	public void onReceive(Context context, Intent intent) {
		if (ACTION_BOOT.equals(intent.getAction()) || ACTION.equals(intent.getAction()) ){
			System.out.println("BootCompletedSensorReceiver  start SensorService");
			Log.i(TAG, "BootCompletedSensorReceiver  start SensorService");
			//context.startService(new Intent(context,LightSensorService.class)); 
			startPollingService(context, 60, LightSensorService.class, SERVICE_ACTION);
		}
		
	}
	
	//开启轮询服务  
    public static void startPollingService(Context context, int seconds, Class<?> cls,String action) {  
        //获取AlarmManager系统服务  
        AlarmManager manager = (AlarmManager) context  
                .getSystemService(Context.ALARM_SERVICE);  
          
        //包装需要执行Service的Intent  
        Intent intent = new Intent(context, cls);  
        intent.setAction(action);  
        PendingIntent pendingIntent = PendingIntent.getService(context, 0,  
                intent, PendingIntent.FLAG_UPDATE_CURRENT);  
          
        //触发服务的起始时间  
        long triggerAtTime = SystemClock.elapsedRealtime();  
          
        //使用AlarmManger的setRepeating方法设置定期执行的时间间隔（seconds秒）和需要执行的Service  
        manager.setRepeating(AlarmManager.ELAPSED_REALTIME, triggerAtTime,  
                seconds * 1000, pendingIntent);  
    } 

}
