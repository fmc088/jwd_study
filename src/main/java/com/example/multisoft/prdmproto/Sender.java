package com.example.multisoft.prdmproto;

import java.io.ByteArrayOutputStream;
import java.io.DataInputStream;
import java.net.URI;

import org.apache.http.HttpResponse;
import org.apache.http.auth.AuthScope;
import org.apache.http.auth.NTCredentials;
import org.apache.http.auth.UsernamePasswordCredentials;
import org.apache.http.client.methods.HttpGet;
import org.apache.http.client.methods.HttpPost;
import org.apache.http.client.params.AuthPolicy;
import org.apache.http.client.params.HttpClientParams;
import org.apache.http.conn.ClientConnectionManager;
import org.apache.http.conn.params.ConnManagerPNames;
import org.apache.http.conn.params.ConnPerRouteBean;
import org.apache.http.conn.scheme.PlainSocketFactory;
import org.apache.http.conn.scheme.Scheme;
import org.apache.http.conn.scheme.SchemeRegistry;
import org.apache.http.conn.ssl.SSLSocketFactory;
import org.apache.http.entity.ByteArrayEntity;
import org.apache.http.impl.auth.BasicSchemeFactory;
import org.apache.http.impl.auth.DigestSchemeFactory;
import org.apache.http.impl.client.DefaultHttpClient;
import org.apache.http.impl.conn.tsccm.ThreadSafeClientConnManager;
import org.apache.http.params.BasicHttpParams;
import org.apache.http.params.HttpConnectionParams;
import org.apache.http.params.HttpParams;
import org.apache.http.params.HttpProtocolParams;
import org.apache.http.util.EntityUtils;

public class Sender {


	private static DefaultHttpClient DefaultHttpClientFactory(String domain, String username, String password) throws Exception {
		HttpParams _params = new BasicHttpParams();
		HttpClientParams.setRedirecting(_params, true);
		HttpConnectionParams.setConnectionTimeout(_params, 60000);
		HttpConnectionParams.setSoTimeout(_params, 60000);
		
		SchemeRegistry schemeRegistry = new SchemeRegistry();
		schemeRegistry.register(new Scheme("http", PlainSocketFactory.getSocketFactory(), 80));
		schemeRegistry.register(new Scheme("https", SSLSocketFactory.getSocketFactory(), 443));
		
		_params.setParameter(ConnManagerPNames.MAX_TOTAL_CONNECTIONS, 30);
		_params.setParameter(ConnManagerPNames.MAX_CONNECTIONS_PER_ROUTE, new ConnPerRouteBean(30));
		_params.setParameter(HttpProtocolParams.USE_EXPECT_CONTINUE, false);

		ClientConnectionManager cm = new ThreadSafeClientConnManager(_params, schemeRegistry);

		final DefaultHttpClient httpClient = new DefaultHttpClient(cm, _params);
		if(username != null){
			httpClient.getAuthSchemes().register(AuthPolicy.NTLM, new NTLMSchemeFactory());
			httpClient.getCredentialsProvider().setCredentials(new AuthScope(null, -1, null, "ntlm"), new NTCredentials(username, password, "", domain));

			httpClient.getAuthSchemes().register(AuthPolicy.BASIC, new BasicSchemeFactory());
			httpClient.getCredentialsProvider().setCredentials(new AuthScope(null, -1, null, "basic"), new UsernamePasswordCredentials((domain.equals("") ? "" : domain + "\\") + username, password));

			httpClient.getAuthSchemes().register(AuthPolicy.DIGEST, new DigestSchemeFactory());
			httpClient.getCredentialsProvider().setCredentials(new AuthScope(null, -1, null, "digest"), new NTCredentials(username, password, "", domain));
		}

		return httpClient;
	}

	
	public static byte[] Send(URI url, byte[] buffer, String domain, String user, String password) throws Exception
	{
		DefaultHttpClient client = DefaultHttpClientFactory(domain, user, password);
		
		HttpPost httppost = new HttpPost(url);
		
		ByteArrayEntity reqEntity = new ByteArrayEntity(buffer);
	    httppost.setEntity(reqEntity);
	    HttpResponse response = client.execute(httppost);
	    response.getStatusLine().getStatusCode();
		//Handle errors differently
		if (response.getStatusLine().getStatusCode() != 200)
		{
			String msg;
			try {
				// In case of error the server should send some error messages in the ErrorStream 
				/* The current server prototype sends various errors including the following:  				 
				 
				1.	HTTP POST processing
					a.	return Request.CreateErrorResponse (HttpStatusCode.BadRequest, "required 'multipart/form-data' content");
					b.	return Request.CreateErrorResponse (HttpStatusCode.BadRequest, "required ConfigFile part");
				2.	decryption
					a.	throw new InvalidDataException ("Invalid configuration file prefix");
					b.	throw new InvalidDataException ("Invalid configuration file version");
				3.	XML parsing
					a.	standard .NET exception
				4.	XML processing
					a.	throw new ApplicationException ("Invalid direction in configuration: " + direction);
					b.	throw new ApplicationException ("invalid configuration type in configuration: " + configurationType);
					c.	throw new ApplicationException ("invalid file version in configuration: " + fileVersion);
					d.	throw new ApplicationException ("missing required attribute: " + attributeName);
					e.	throw new ApplicationException ("missing required attribute value: " + attributeName);
				5.	Prototype temporary processing (save as file)
					a.	standard .NET exception
				 */							
				
				msg = "" + response.getStatusLine().getStatusCode() + " " + response.getStatusLine().getReasonPhrase();
				try
				{
					msg += "\n" + EntityUtils.toString(response.getEntity(), "UTF-8");
				}
				catch(Exception e){
				}
			} catch (Exception e) {
				msg = "Failed to getReasonPhrase from response :" + e.getMessage();
			}
			throw new Exception("Server Responded with Error :" + msg );
		}
		//In case there is no error (200 == OK) we should read the input stream for the response (encrypted xml with prefix)
		DataInputStream in = new DataInputStream(response.getEntity().getContent());
		ByteArrayOutputStream b = new ByteArrayOutputStream();
		byte[] readbuffer = new byte[4096];
		int n;
		while ( (n = in.read(readbuffer)) != -1)
		{
		    if (n > 0)
		    {
		        b.write(readbuffer, 0, n);
		    }
		}
		return b.toByteArray();
	}

	public static byte[] Get(URI url, String domain, String user, String password) throws Exception
	{
		DefaultHttpClient client = DefaultHttpClientFactory(domain, user, password);

		HttpGet httpget = new HttpGet(url);

		HttpResponse response = client.execute(httpget);
		response.getStatusLine().getStatusCode();
		//Handle errors differently
		if (response.getStatusLine().getStatusCode() != 200)
		{
			String msg;
			try {
				// In case of error the server should send some error messages in the ErrorStream
				/* The current server prototype sends various errors including the following:

				1.	HTTP POST processing
					a.	return Request.CreateErrorResponse (HttpStatusCode.BadRequest, "required 'multipart/form-data' content");
					b.	return Request.CreateErrorResponse (HttpStatusCode.BadRequest, "required ConfigFile part");
				2.	decryption
					a.	throw new InvalidDataException ("Invalid configuration file prefix");
					b.	throw new InvalidDataException ("Invalid configuration file version");
				3.	XML parsing
					a.	standard .NET exception
				4.	XML processing
					a.	throw new ApplicationException ("Invalid direction in configuration: " + direction);
					b.	throw new ApplicationException ("invalid configuration type in configuration: " + configurationType);
					c.	throw new ApplicationException ("invalid file version in configuration: " + fileVersion);
					d.	throw new ApplicationException ("missing required attribute: " + attributeName);
					e.	throw new ApplicationException ("missing required attribute value: " + attributeName);
				5.	Prototype temporary processing (save as file)
					a.	standard .NET exception
				 */

				msg = "" + response.getStatusLine().getStatusCode() + " " + response.getStatusLine().getReasonPhrase();
				try
				{
					msg += "\n" + EntityUtils.toString(response.getEntity(), "UTF-8");
				}
				catch(Exception e){
				}
			} catch (Exception e) {
				msg = "Failed to getReasonPhrase from response :" + e.getMessage();
			}
			throw new Exception("Server Responded with Error :" + msg );
		}
		//In case there is no error (200 == OK) we should read the input stream for the response (encrypted xml with prefix)
		DataInputStream in = new DataInputStream(response.getEntity().getContent());
		ByteArrayOutputStream b = new ByteArrayOutputStream();
		byte[] readbuffer = new byte[4096];
		int n;
		while ( (n = in.read(readbuffer)) != -1)
		{
			if (n > 0)
			{
				b.write(readbuffer, 0, n);
			}
		}
		return b.toByteArray();
	}
}

