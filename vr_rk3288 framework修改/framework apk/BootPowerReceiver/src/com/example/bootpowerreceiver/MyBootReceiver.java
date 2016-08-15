package com.example.bootpowerreceiver;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;

public class MyBootReceiver extends BroadcastReceiver {
	@Override
	public void onReceive(Context context, Intent intent) {
		// TODO Auto-generated method stub
		String action = intent.getAction();
		if (Intent.ACTION_BOOT_COMPLETED.equals(action)) {
			//context.startService(new Intent(context,PowerSensorService.class)); 
		}
	}
}
