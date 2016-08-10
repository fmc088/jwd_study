package com.android.settings.deviceinfo;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.util.Log;

public class ShutDownReceiver extends BroadcastReceiver{

	@Override
	public void onReceive(Context context, Intent intent) {
		// TODO Auto-generated method stub
		Log.i("ShutDowReceiver", "onReceive");
		String action = intent.getAction();
		if(Intent.ACTION_SHUTDOWN.equals(action)){
			context.stopService(new Intent(context,PowerSensorService.class));
		}
		
	}

}
