package com.example.mysensordemo;

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
import android.util.Log;

public class LightSensorService extends Service implements SensorEventListener {
	// 调用距离传感器，控制屏幕
	private SensorManager mManager;// 传感器管理对象
	private PowerManager localPowerManager = null;// 电源管理对象
	private PowerManager.WakeLock localWakeLock = null;// 电源锁
	private static boolean currentbright = true; // 判断当前亮状态
	private final static String TAG = LightSensorService.class.getSimpleName();
	Handler handler=new Handler();  
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
				SensorManager.SENSOR_DELAY_NORMAL);// 注册传感器，第一个参数为距离监听器，第二个是传感器类型，第三个是延迟类型
		if (localPowerManager == null) {
			// 获取系统服务POWER_SERVICE，返回一个PowerManager对象
			localPowerManager = (PowerManager) getSystemService(Context.POWER_SERVICE);
		}
		if (localWakeLock == null) {
			// 获取PowerManager.WakeLock对象,后面的参数|表示同时传入两个值,最后的是LogCat里用的Tag
			localWakeLock = this.localPowerManager.newWakeLock(32, "MyPower");// 第一个参数为电源锁级别，第二个是日志tag
		}

	}
	@Override
	public void onCreate() {
		super.onCreate();
		Log.i(TAG, "onCreate()");
		initView();
	}
	@Override
	public void onDestroy() {
		super.onDestroy();
		Log.i(TAG, "onDestroy()");

		if (mManager != null) {
			localWakeLock.release();// 释放电源锁，如果不释放finish这个acitivity后仍然会有自动锁屏的效果，不信可以试一试
			mManager.unregisterListener(this);// 注销传感器监听
		}
		Intent intent = new Intent("com.destroy.restart.service");  
	    sendBroadcast(intent);  
	}
	@Override
	public void onSensorChanged(SensorEvent event) {
		float[] its = event.values;
		//Log.i(TAG, "onSensorChanged currentbright = "+currentbright);
		if (its != null && event.sensor.getType() == Sensor.TYPE_PROXIMITY) {
			if (its[0] > 128) {// 靠近 亮
				//Log.i(TAG, "  its[0] > 128 onSensorChanged currentbright = "+currentbright);
				if (!currentbright) {
					//Log.i(TAG, "  its[0] > 128 onSensorChanged localWakeLock ");
					if (localWakeLock.isHeld()) {
						return;
					} else {
						currentbright = true;
						localWakeLock.acquire();// 申请设备电源锁 锁屏
						
						//Log.i(TAG, "  its[0] > 128 onSensorChanged  localWakeLock currentbright = "+currentbright);
						
					}
				}
			} else {// 远离 灭
				//Log.i(TAG, "  else onSensorChanged currentbright = "+currentbright);
				if (currentbright) {
					//Log.i(TAG, "  else onSensorChanged localWakeLock ");
					if (localWakeLock.isHeld()) {
						return;
					} else {
						//Log.i(TAG, "  else onSensorChanged localWakeLock ");
						currentbright = false;
						localWakeLock.setReferenceCounted(false);
						
						handler.postDelayed(new Runnable(){
			                public void run(){
			                	localWakeLock.release(); // 释放设备电源锁
			                }
			            }, 10*100);
						
						//Log.i(TAG, "  else onSensorChanged localWakeLock currentbright = "+currentbright);
						
					}
				}

			}
		}
	}
	@Override
	public void onAccuracyChanged(Sensor sensor, int accuracy) {
		// TODO Auto-generated method stub

	}
}
