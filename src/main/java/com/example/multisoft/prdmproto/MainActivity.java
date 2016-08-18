package com.example.multisoft.prdmproto;

import java.io.ByteArrayInputStream;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.InputStream;
import java.net.URI;


import android.app.Activity;
import android.app.AlertDialog;

import android.os.AsyncTask;
import android.os.Bundle;
import android.os.Environment;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.EditText;
import javax.xml.parsers.DocumentBuilderFactory;
import javax.xml.parsers.DocumentBuilder;


import org.w3c.dom.Document;
import org.w3c.dom.Element;
import org.w3c.dom.Node;
import org.w3c.dom.NodeList;
import org.xml.sax.InputSource;
import java.io.File;

public class MainActivity extends Activity {
    public static String TAG = "MainActivity";

    /// <summary>
    /// Hard coded encryption key
    /// </summary>
    private static byte[] _TheKey = new byte[] { (byte)222, (byte)251, 97, 8, (byte)239, 70, 107, 62, (byte)131, (byte)209, (byte)169, 24, (byte)247, 4, 65, 77, (byte)139, (byte)235, 22, 124, 4, (byte)205, (byte)169, (byte)163, 4, 0, 2, 125, (byte)255, (byte)247, 8, 30 };

    /// <summary>
    /// Prefix bytes of the configuration file
    /// </summary>
    private static byte[] _HeaderPrefixTypeSpeechAirOS = new byte[] { 80, 83, 65, 67 }; // 'PSAC' = Philips SpeechAir Configuration
    private static byte[] _HeaderPrefixTypePhilipsDictationRecorderSpeechAir = new byte[] { 80, 68, 82, 83 }; // 'PDRS' = Philips Dictation Recorder SpeechAir
    private static byte[] _HeaderPrefixTypePhilipsDictationRecorderAndroid = new byte[] { 80, 68, 82, 65 }; // 'PDRA' = Philips Dictation Recorder Android
    /// <summary>
    /// Supported file version
    /// </summary>
    private static byte _FileVersion = 2;
    // SP 配置文件存放的路径
    public static String ROOT_HARDDISK_DIR = Environment
            .getExternalStorageDirectory().getAbsolutePath();
    public static String SPS_CONFIG_FILES_PATH = ROOT_HARDDISK_DIR
            + File.separator + "SAconfig";
    public static String SPS_USB_CONFIG_FILE_PATH_ORG = SPS_CONFIG_FILES_PATH
            + File.separator + "SAin.config";
   // public static String SPS_USB_CONFIG_FILE_PATH_OUT = SPS_CONFIG_FILES_PATH
      //     + File.separator + "speechairosout.xml";
     public static String SPS_USB_CONFIG_FILE_PATH_OUT = SPS_CONFIG_FILES_PATH
          + File.separator + "SAout.config";

    private Handler myHandler = new Handler(){
        @Override
        public void handleMessage(Message msg) {
            super.handleMessage(msg);
            xmledit.setText(xml);
        }
    };
    private class SendAndReceiveTask extends AsyncTask<String, Integer, byte[]> {
        public Button sendButton;
        public Button getFileButton;
        public EditText fileUrlText;
        public EditText originalExtensionText;

        Exception exception;
        protected byte[] doInBackground(String... params ) {

            try
            {
                exception = null;
                byte[] encrypted = Cryptor.encryptWithHeader(_TheKey, _HeaderPrefixTypeSpeechAirOS, _FileVersion, params[1]);
                //byte[] encrypted = (byte[]) params[1].getBytes();
                byte[] received = Sender.Send(new URI(params[0]), encrypted,(String)params[2],(String)params[3],(String)params[4]);
                return Cryptor.decryptWithHeader(_TheKey, _HeaderPrefixTypeSpeechAirOS, _FileVersion, received);
            }
            catch (Exception e)
            {
                exception = e;
            }
            return null;
        }
        protected void onPostExecute (byte[] result)
        {
            if (sendButton!=null)
            {
                sendButton.setEnabled(true);
                getFileButton.setEnabled(true);
            }
            AlertDialog.Builder dlgAlert  = new AlertDialog.Builder(MainActivity.this);
            if (exception != null)
            {

                dlgAlert.setMessage(exception.toString());
                dlgAlert.setTitle("Exception");
            }
            else
            {
                dlgAlert.setMessage(new String(result));
                dlgAlert.setTitle("Result");
            }
            dlgAlert.setPositiveButton("OK", null);
            dlgAlert.create().show();

            if (exception!=null)
                return;

            //get the RelativeURL of the Wallpaper from the result
            DocumentBuilderFactory factory;
            DocumentBuilder builder;
            InputStream is;
            Document dom;
            try {
                is = new ByteArrayInputStream(result);
                factory = DocumentBuilderFactory.newInstance();
                builder = factory.newDocumentBuilder();
                dom = builder.parse(is);
                NodeList nodes = dom.getDocumentElement().getElementsByTagName("Setting");
                for (int j = 0; j<nodes.getLength(); j++)
                {
                    Element n = (Element)nodes.item(j);
                    String name = n.getAttribute("ID");
                    if (name.equals("Wallpaper"))
                    {
                        fileUrlText.setText(n.getAttribute("RelativeURL"));
                        originalExtensionText.setText(n.getAttribute("OriginalExtension"));
                    }
                }
            }
            catch (Exception e)
            {
                dlgAlert  = new AlertDialog.Builder(MainActivity.this);
                dlgAlert.setMessage(e.toString());
                dlgAlert.setTitle("Exception trying to get Wallpaper");
                dlgAlert.setPositiveButton("OK", null);
                dlgAlert.create().show();
            }
        }
    }

    public boolean decryptFile(){
        File orgFile = null;
        FileInputStream fis = null;
        orgFile = new File(SPS_USB_CONFIG_FILE_PATH_OUT );
        if(!orgFile.exists()){
            Log.i(TAG,"decriptFile file : " + SPS_USB_CONFIG_FILE_PATH_OUT + " is not exists or not a file");
            return false;
        }
        try {
            fis = new FileInputStream(orgFile);
            byte[] orgFileByte = new byte[(int)orgFile.length()];
            fis.read(orgFileByte);
            fis.close();
            xml = new String(Cryptor.decryptWithHeader(_TheKey, _HeaderPrefixTypeSpeechAirOS, _FileVersion, orgFileByte));
            Log.i(TAG,"xml-----: "+xml +" \r\n str = "+new String(Cryptor.decryptWithHeader(_TheKey, _HeaderPrefixTypeSpeechAirOS, _FileVersion, orgFileByte)));
            myHandler.sendEmptyMessage(0);
            return true;
        }catch (Exception e){
            Log.e(TAG,"Exception:"+e.getMessage());
            return false;
        }
    }
    String xml;
    EditText xmledit;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        final Button sendButton = (Button) findViewById(R.id.sendButton);
        final EditText urlText= (EditText) findViewById(R.id.serverURLedit);
        xmledit= (EditText) findViewById(R.id.xmlEdit);
        final EditText domainedit = (EditText) findViewById(R.id.DomainEdit);
        final EditText useredit = (EditText) findViewById(R.id.UserEdit);
        final EditText passwordedit = (EditText) findViewById(R.id.PasswordEdit);

        final Button getFileButton = (Button) findViewById(R.id.getFileButton);
        final EditText fileUrlText = (EditText) findViewById(R.id.fileUrlText);
        final EditText originalExtensionText = (EditText) findViewById(R.id.originalExtensionText);

//        try
//        {
//            FileInputStream fis = new FileInputStream(SPS_USB_CONFIG_FILE_PATH_OUT);
//            //InputStream fis = getResources().openRawResource(R.raw.speechairosout);
//            StringBuffer fileContent = new StringBuffer("");
//
//            byte[] buffer = new byte[1024];
//            int n;
//            while ((n = fis.read(buffer)) != -1)
//            {
//                fileContent.append(new String(buffer, 0, n));
//            }
//            fis.close();
//            xml = fileContent.toString();
//
//        }
//        catch(Exception e)
//        {
//            xml = e.toString();
//        }

        sendButton.setOnClickListener(new OnClickListener() {
            public void onClick(View v) {
                Log.i(TAG,"decript setOnClickListener start " );
                decryptFile();
                //While the sending is going on in the background the Send button is disabled
//                sendButton.setEnabled(false);
//                getFileButton.setEnabled(false);
//                SendAndReceiveTask t = new SendAndReceiveTask();
//                t.sendButton=sendButton;
//                t.getFileButton=getFileButton;
//                t.fileUrlText = fileUrlText;
//                t.originalExtensionText = originalExtensionText;
//                t.execute(urlText.getText().toString() + "/api/PRDMConfiguration",
//                        xmledit.getText().toString(),
//                        domainedit.getText().toString(),
//                        useredit.getText().toString(),
//                        passwordedit.getText().toString());
            }
        });

        getFileButton.setOnClickListener(new OnClickListener() {
            public void onClick(View v) {
                //While the sending is going on in the background the Send button is disabled
                sendButton.setEnabled(false);
                getFileButton.setEnabled(false);
                GetFileTask t = new GetFileTask();
                t.sendButton=sendButton;
                t.getFileButton=getFileButton;
                t.execute(urlText.getText().toString() + "/" + fileUrlText.getText(),
                        originalExtensionText.getText().toString(),
                        domainedit.getText().toString(),
                        useredit.getText().toString(),
                        passwordedit.getText().toString());
            }
        });
    }
    private class GetFileTask extends AsyncTask<String, Integer, byte[]> {
        public Button sendButton;
        public Button getFileButton;

        Exception exception;
        protected byte[] doInBackground(String... params ) {

            try
            {
                exception = null;
                byte[] result = Sender.Get(new URI(params[0]),(String)params[2],(String)params[3],(String)params[4]);
                FileOutputStream fs = new FileOutputStream(Environment.getExternalStorageDirectory().toString() + "/result." + params[1]);
                fs.write(result);
                fs.close();
                return  null;
            }
            catch (Exception e)
            {
                exception = e;
            }
            return null;
        }
        protected void onPostExecute (byte[] result)
        {
            sendButton.setEnabled(true);
            getFileButton.setEnabled(true);

            AlertDialog.Builder dlgAlert  = new AlertDialog.Builder(MainActivity.this);
            if (exception != null)
            {
                dlgAlert.setMessage(exception.toString());
                dlgAlert.setTitle("Exception");
            }
            else
            {
                dlgAlert.setMessage("file saved to external storage");
                dlgAlert.setTitle("Result");
            }
            dlgAlert.setPositiveButton("OK", null);
            dlgAlert.create().show();
        }
    }

}
