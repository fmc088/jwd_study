package com.example.bootpowerreceiver;

import java.io.BufferedReader;
import java.io.FileReader;
import java.io.IOException;

import android.app.Service;
import android.content.Context;
import android.content.Intent;
import android.os.IBinder;
import android.os.PowerManager;
import android.os.PowerManager.WakeLock;
import android.os.SystemClock;
import android.util.Log;

public class PowerSensorService extends Service{
	private PowerManager pm = null;// 电源管理对象
	private WakeLock wl = null;//电源锁
	String path = "/sys/devices/ff100000.adc/key.30/get_gsensor_value";//节点文件路径
	private final static String TAG = PowerSensorService.class.getSimpleName();//打印标签
	public boolean isGo = true;//循环读取文件节点
	public PowerThread powerThread;//读取节点线程
	
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
	
	@Override
	public void onCreate() {
		super.onCreate();
		Log.i(TAG, "onCreate()");
		initView();
		powerThread = new PowerThread();
		isGo = true;
		powerThread.start();
		
	}
	public void initView() {
		if (pm == null) {
			// 获取系统服务POWER_SERVICE，返回一个PowerManager对象
			pm = (PowerManager) getSystemService(Context.POWER_SERVICE);
		}
		if (wl == null) {
			wl = pm.newWakeLock(
					PowerManager.PARTIAL_WAKE_LOCK, getClass() 
                    .getCanonicalName());
		}
		wl.setReferenceCounted(false);
		if(!wl.isHeld()){
			wl.acquire();
		}
		
	}
	@Override
	public void onDestroy() {
		super.onDestroy();
		
		closewakeup();
		
		isGo = false;
		if(powerThread != null){
			powerThread.interrupt();
			powerThread = null;
		}
		Log.i(TAG, "onDestroy()");
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
	public class PowerThread extends Thread{
		@Override
		public void run() {
			// TODO Auto-generated method stub
			super.run();
			while(isGo){
				try {
					Thread.sleep(1000);
				} catch (InterruptedException e) {
					e.printStackTrace();
				}
				try {
					String str = mygetString(path);
					if("0".equals(str.trim())){ //读取0时亮屏
						initView();
						if(!pm.isScreenOn()){
							if(null != pm){
								pm.wakeUp(SystemClock.uptimeMillis());
							}
						}
					}else if("fail".equals(str.trim())){//读取节点异常
						Log.i(TAG, "----mygetString IOException---");
					}else if("1".equals(str.trim())){//读取到1时屏幕灭屏，
						closewakeup();
						if(pm.isScreenOn()){
				            if(null != pm){
				            	pm.goToSleep(SystemClock.uptimeMillis());
				            }
						}
					}else{
						Log.i(TAG, "---PowerThread get the error data " +str);
					}
				} catch (Exception e) {
					Log.e(TAG,"PowerThread :"+ e.getMessage());
					e.printStackTrace();
				}
			}
		}
	}
	private void closewakeup(){
		if(wl != null){
			if(wl.isHeld()){
				wl.release();
				wl = null;
			}
		}
	}
}
