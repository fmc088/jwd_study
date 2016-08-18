package com.example.multisoft.prdmproto;
import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.security.InvalidAlgorithmParameterException;
import java.security.InvalidKeyException;
import java.security.NoSuchAlgorithmException;

import javax.crypto.BadPaddingException;
import javax.crypto.Cipher;
import javax.crypto.IllegalBlockSizeException;
import javax.crypto.NoSuchPaddingException;
import javax.crypto.spec.IvParameterSpec;
import javax.crypto.spec.SecretKeySpec;
public class Cryptor {
	
	static byte[] iv = new byte[] { (byte)198, 1, 110, 71, 97, 50, 60, 41, 97, 50, 40, 5, (byte)197, 90, 1, 12 }; 

	public static byte[] encryptWithHeader(byte[] key, byte[] fileHeaderPrefix, byte fileVersion, String xml)
			throws NoSuchAlgorithmException, NoSuchPaddingException,
			InvalidKeyException, IOException, IllegalBlockSizeException,
			BadPaddingException, InvalidAlgorithmParameterException {
		
		ByteArrayOutputStream bb = new ByteArrayOutputStream();
		bb.write(fileHeaderPrefix);
		bb.write(fileVersion);
		Cipher cipher = Cipher.getInstance("AES/CBC/PKCS7Padding");
		//iv will be set to 16 zeroes, otherwise it would be random and would not work. So, right now we also have a hard coded initialisation vector
		
		cipher.init(Cipher.ENCRYPT_MODE, new SecretKeySpec(key, "AES"), new IvParameterSpec(iv));

		bb.write(cipher.doFinal(xml.getBytes()));
		return bb.toByteArray();
	}
	
	public static byte[] decryptWithHeader(byte[] key, byte[] fileHeaderPrefix, byte fileVersion, byte[] encriptedXml)
			throws Exception {
		int i;
		//We check the file prefix and encryption version coming from the server
		for(i = 0; i <fileHeaderPrefix.length; i++)
		{
			if(fileHeaderPrefix[i]!= encriptedXml[i])
				throw new Exception("Response header prefix mismatch");
		}
		if (fileVersion!=encriptedXml[i++])
		{
			throw new Exception("Response header version mismatch");		
		}

		Cipher cipher = Cipher.getInstance("AES/CBC/PKCS7Padding");
		//iv will be set to 16 zeroes, otherwise it would be random. So, right now we also have a hard coded initialisation vector
		cipher.init(Cipher.DECRYPT_MODE, new SecretKeySpec(key, "AES"), new IvParameterSpec(iv));
		//decrypt everything that comes after the header
		return cipher.doFinal(encriptedXml,i,encriptedXml.length-i);
	}
}
