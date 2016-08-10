package com.android.settings.deviceinfo;

import java.io.BufferedReader;
import java.io.FileReader;
import java.io.IOException;

import android.app.Service;
import android.content.Context;
import android.content.Intent;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.os.IBinder;
import android.os.PowerManager;
import android.os.PowerManager.WakeLock;
import android.os.SystemClock;
import android.util.Log;

public class PowerSensorService extends Service implements SensorEventListener {
	// 调用距离传感器，控制屏幕
	//private SensorManager mManager;// 传感器管理对象
	private PowerManager localPowerManager = null;// 电源管理对象
	private WakeLock localWakeLock = null;// 电源锁
	private WakeLock relaseWakeLock = null;
	String path = "/sys/devices/ff100000.adc/key.30/get_gsensor_value";
	private final static String TAG = PowerSensorService.class.getSimpleName();
	private final static String ACTION = "com.AlarmBroadCastReceiver";
	
	public boolean isGo = true;

	@Override
	public int onStartCommand(Intent intent, int flags, int startId) {
		
		// TODO Auto-generated method stub
		return super.onStartCommand(intent, flags, startId);
	}

	@Override
	public IBinder onBind(Intent intent) {
		// TODO Auto-generated method stub
		return null;
	}
	public void initView() {
//		if (mManager == null) {
//			mManager = (SensorManager) getSystemService(Context.SENSOR_SERVICE);
//		}
//		mManager.registerListener(this,
//				mManager.getDefaultSensor(Sensor.TYPE_PROXIMITY),// 距离感应器
//				SensorManager.SENSOR_DELAY_UI);// 注册传感器，第一个参数为距离监听器，第二个是传感器类型，第三个是延迟类型
		if (localPowerManager == null) {
			// 获取系统服务POWER_SERVICE，返回一个PowerManager对象
			localPowerManager = (PowerManager) getSystemService(Context.POWER_SERVICE);
		}
		if (localWakeLock == null) {
			// 获取PowerManager.WakeLock对象,后面的参数|表示同时传入两个值,最后的是LogCat里用的Tag
//			localWakeLock = this.localPowerManager.newWakeLock(
//					PowerManager.FULL_WAKE_LOCK, getClass() 
//                    .getCanonicalName());// 第一个参数为电源锁级别，第二个是日志tag
			
			relaseWakeLock = this.localPowerManager.newWakeLock(
					PowerManager.PARTIAL_WAKE_LOCK, getClass() 
                    .getCanonicalName());// 第一个参数为电源锁级别，第二个是日志tag
			relaseWakeLock.acquire();
			relaseWakeLock.setReferenceCounted(false);
		}
	}
	@Override
	public void onCreate() {
		super.onCreate();
		Log.i(TAG, "onCreate()");
		initView();
		powerThread = new PowerThread();
		isGo = true;
		powerThread.start();
		
	}
	PowerThread powerThread;
	@Override
	public void onDestroy() {
		super.onDestroy();
		
//		if (mManager != null) {
//			mManager.unregisterListener(this);// 注销传感器监听
//		}
		if(relaseWakeLock != null){
			relaseWakeLock.release();
			Log.i(TAG, "-------------release()-------");
		}
		isGo = false;
		if(powerThread != null){
			powerThread.interrupt();
			powerThread = null;
		}
		Log.i(TAG, "onDestroy()");
	}
	@Override
	public void onSensorChanged(SensorEvent event) {
		float[] its = event.values;
		 Log.i(TAG, "onSensorChanged currentdata = " + its[0]);
		if (its != null && event.sensor.getType() == Sensor.TYPE_PROXIMITY) {
			if (its[0] == 0) {// 靠近 亮
				Log.i(TAG, "onSensorChanged show = "+localPowerManager.isScreenOn());
				if(!localPowerManager.isScreenOn()){
					localPowerManager.wakeUp(SystemClock.uptimeMillis());
				}
			} else {// 远离 灭
				Log.i("MainActivity", "goTosleep");
				if(localPowerManager.isScreenOn()){
					Log.i("MainActivity", "goTosleep");
		            if(null != localPowerManager){
		            	localPowerManager.goToSleep(SystemClock.uptimeMillis());
		                Log.i("MainActivity", "goTosleep");
		            }
				}
				
			}
		}
	}
	@Override
	public void onAccuracyChanged(Sensor sensor, int accuracy) {
		// TODO Auto-generated method stub

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
		Log.i(TAG, "----prop -------="+prop+"----");
		return prop;
	}
	public class PowerThread extends Thread{
		@Override
		public void run() {
			// TODO Auto-generated method stub
			super.run();
			while(isGo){
				Log.i(TAG, "---PowerThread ---start----");
				try {
					Thread.sleep(1000);
				} catch (InterruptedException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
				try {
					String str = mygetString(path);
					Log.i(TAG, "String getString ="+str+"---");
					if("0".equals(str.trim())){ //读取0时亮屏
						Log.i(TAG, "onSensorChanged show = "+localPowerManager.isScreenOn());
						if(!localPowerManager.isScreenOn()){
							if(null != localPowerManager){
								localPowerManager.wakeUp(SystemClock.uptimeMillis());
							}
						}
					}else if("fail".equals(str.trim())){
						Log.i(TAG, "mygetString fail");
					}else if("1".equals(str.trim())){//读取到1时屏幕灭屏，
						Log.i(TAG, "---PowerThread isScreenOn ="+localPowerManager.isScreenOn());
						if(localPowerManager.isScreenOn()){
							Log.i(TAG, "goTosleep");
				            if(null != localPowerManager){
				            	localPowerManager.goToSleep(SystemClock.uptimeMillis());
				                Log.i(TAG, "goTosleep");
				            }
						}
					}else{//如果设备传感器失灵，亮起灯
						Log.i(TAG, "---PowerThread mygetString data error or sensor bad");
						if(!localPowerManager.isScreenOn()){
							if(null != localPowerManager){
								localPowerManager.wakeUp(SystemClock.uptimeMillis());
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
