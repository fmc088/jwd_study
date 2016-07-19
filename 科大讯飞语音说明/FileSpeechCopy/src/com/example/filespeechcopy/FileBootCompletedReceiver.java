package com.example.filespeechcopy;

import java.io.File;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.os.Environment;
import android.util.Log;

public class FileBootCompletedReceiver extends BroadcastReceiver{
	private final String ACTION_BOOT = "android.intent.action.BOOT_COMPLETED";
	private FileCopyPublic fCopyPublic;
	 private static final String resPath = Environment
				.getExternalStorageDirectory().getAbsolutePath()+"/";
	@Override
	public void onReceive(Context context, Intent intent) {
//		get
		if (ACTION_BOOT.equals(intent.getAction())){
			boolean isExist = new File(resPath+"iflytek").exists();
			Log.i("FileBootCompletedReceiver", "iflytek exist = "+isExist);
			if(!isExist){
				fCopyPublic = new FileCopyPublic(context);
				new Thread(){
					@Override
					public void run() {
						// TODO Auto-generated method stub
						super.run();
						fCopyPublic.CopyAssets("", resPath);
					}
				}.start();
				
			}
		}
		
	}

}
