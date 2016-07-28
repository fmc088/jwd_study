package com.example.bootpowerreceiver;

import android.app.Service;
import android.content.Context;
import android.content.Intent;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.os.Handler;
import android.os.IBinder;
import android.os.PowerManager;
import android.os.PowerManager.WakeLock;
import android.os.SystemClock;
import android.util.Log;

public class PowerSensorService extends Service implements SensorEventListener {
	// 调用距离传感器，控制屏幕
	private SensorManager mManager;// 传感器管理对象
	private PowerManager localPowerManager = null;// 电源管理对象
	private WakeLock localWakeLock = null;// 电源锁
	private WakeLock relaseWakeLock = null;
	private final static String TAG = PowerSensorService.class.getSimpleName();
	private final static String ACTION = "com.AlarmBroadCastReceiver";
	Handler handler = new Handler();

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
		if (mManager == null) {
			mManager = (SensorManager) getSystemService(Context.SENSOR_SERVICE);
		}
		mManager.registerListener(this,
				mManager.getDefaultSensor(Sensor.TYPE_PROXIMITY),// 距离感应器
				SensorManager.SENSOR_DELAY_UI);// 注册传感器，第一个参数为距离监听器，第二个是传感器类型，第三个是延迟类型
		if (localPowerManager == null) {
			// 获取系统服务POWER_SERVICE，返回一个PowerManager对象
			localPowerManager = (PowerManager) getSystemService(Context.POWER_SERVICE);
		}
		if (localWakeLock == null) {
			// 获取PowerManager.WakeLock对象,后面的参数|表示同时传入两个值,最后的是LogCat里用的Tag
			localWakeLock = this.localPowerManager.newWakeLock(
					PowerManager.FULL_WAKE_LOCK, getClass() 
                    .getCanonicalName());// 第一个参数为电源锁级别，第二个是日志tag
			
			relaseWakeLock = this.localPowerManager.newWakeLock(
					PowerManager.PARTIAL_WAKE_LOCK, getClass() 
                    .getCanonicalName());// 第一个参数为电源锁级别，第二个是日志tag
			relaseWakeLock.acquire();
		}
		localWakeLock.setReferenceCounted(false);
		relaseWakeLock.setReferenceCounted(false);
	}
	private PowerManager power;
	@Override
	public void onCreate() {
		super.onCreate();
		Log.i(TAG, "onCreate()");
		power = (PowerManager) this.getSystemService(Context.POWER_SERVICE);
		initView();
	}
	@Override
	public void onDestroy() {
		super.onDestroy();
		Log.i(TAG, "onDestroy()");
		if (mManager != null) {
			//mManager.unregisterListener(this);// 注销传感器监听
		}
//		if(relaseWakeLock != null){
//			relaseWakeLock.
//		}
	}
	@Override
	public void onSensorChanged(SensorEvent event) {
		float[] its = event.values;
		 Log.i(TAG, "onSensorChanged currentdata = " + its[0]);
		if (its != null && event.sensor.getType() == Sensor.TYPE_PROXIMITY) {
			if (its[0] == 0) {// 靠近 亮
				Log.i(TAG, "onSensorChanged show = "+localPowerManager.isScreenOn());
				if(!localPowerManager.isScreenOn()){
					power.wakeUp(SystemClock.uptimeMillis());
				}
//				if(localWakeLock != null){
//					localWakeLock.acquire();
//				}
//				if(relaseWakeLock != null){
//					relaseWakeLock.release();
//				}
//				if(localWakeLock != null){
//					localWakeLock.acquire();
//				}
			} else {// 远离 灭
				Log.i("MainActivity", "goTosleep");
				if(localPowerManager.isScreenOn()){
					
//					if(localWakeLock != null){
//						localWakeLock.release();
//					}
//					if(relaseWakeLock != null){
//						relaseWakeLock.acquire();
//					}
					
//					if(localWakeLock != null){
//						localWakeLock.release();
//					}
					Log.i("MainActivity", "goTosleep");
		            if(null != power){
		                power.goToSleep(SystemClock.uptimeMillis());
		                Log.i("MainActivity", "goTosleep");
		            }
				}
				
			}
		}
	}
	  private void acquireWakeLock() { 
	        if (null == localWakeLock) { 
	            PowerManager pm = (PowerManager) getSystemService(Context.POWER_SERVICE); 
	            localWakeLock = pm.newWakeLock(PowerManager.PARTIAL_WAKE_LOCK 
	                    | PowerManager.ON_AFTER_RELEASE, getClass() 
	                    .getCanonicalName()); 
	            if (null != localWakeLock) { 
	                Log.i(TAG, "call acquireWakeLock"); 
	                localWakeLock.acquire(); 
	            } 
	        } 
	    } 
	 
	    // 释放设备电源锁  
	    private void releaseWakeLock() { 
	        if (null != localWakeLock && localWakeLock.isHeld()) { 
	            Log.i(TAG, "call releaseWakeLock"); 
	            localWakeLock.release(); 
	            localWakeLock = null; 
	        } 
	    }
	
	@Override
	public void onAccuracyChanged(Sensor sensor, int accuracy) {
		// TODO Auto-generated method stub

	}
}
