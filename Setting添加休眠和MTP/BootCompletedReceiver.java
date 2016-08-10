package com.android.settings.deviceinfo;


import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.hardware.usb.UsbManager;
import android.util.Log;

public class BootCompletedReceiver extends BroadcastReceiver{
	private UsbManager mUsbManager;
	@Override
	public void onReceive(Context context, Intent intent) {
//		get
		
		String action = intent.getAction();
		if (Intent.ACTION_BOOT_COMPLETED.equals(action)) { 
			Log.i("BootCompletedReceiver", "onReceive");
			mUsbManager = (UsbManager) context.getSystemService(Context.USB_SERVICE);
			mUsbManager.setCurrentFunction(UsbManager.USB_FUNCTION_MTP, true);
			Log.i("BootCompletedReceiver", "onReceive");
			context.startService(new Intent(context,PowerSensorService.class));
		}
	}

}
