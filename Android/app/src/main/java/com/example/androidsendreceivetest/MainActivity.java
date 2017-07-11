package com.example.androidsendreceivetest;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.view.Window;
import android.widget.Button;
import android.widget.EditText;
import android.widget.Toast;

import org.apache.http.HttpResponse;
import org.apache.http.client.methods.HttpGet;
import org.apache.http.impl.client.DefaultHttpClient;
import org.apache.http.params.HttpConnectionParams;
import org.apache.http.params.HttpParams;
import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import java.io.BufferedReader;
import java.io.InputStreamReader;

/**
 * Created by jeonjinwoo on 2016-12-03.
 */

public class MainActivity extends Activity{

	EditText id;
	Button loginButton;
	String _id;
	Integer int_id;
	Integer[] all_ids = new Integer[100]; // Stored ID collection
    Integer flag2 = 0;

	@Override
	protected void onCreate(Bundle savedInstanceState){
		super.onCreate(savedInstanceState);

		requestWindowFeature(Window.FEATURE_NO_TITLE);// Remove top titlebar
		setContentView(R.layout.activity_main);
		id = (EditText) findViewById(R.id.id);
		loginButton = (Button) findViewById(R.id.loginButton);
		loginButton.setOnClickListener(new View.OnClickListener(){//button click method
			@Override
			public void onClick (View view) {//pass value to showactivity and go to showactivity

				_id = id.getText().toString();//id to string
				_id = _id.trim();//can't login when enter the space

				if (_id.getBytes().length <= 0){
					flag2 = 2;
					Toast toast = Toast.makeText(MainActivity.this, "값을 입력하세요.", Toast.LENGTH_SHORT);
					toast.show();
				}
				else {

					Log.d("234d1", "!!!!" + _id);
					int_id = Integer.parseInt(_id);// string to integer
					String result = SendByHttp(null, _id);// send null to server and get the data
					jsonParserList1(result); // JSON parsing
				}
                if(flag2 == 1){// enter id == stored id 입력한 아이디 값과 저장된 아이디 값이 같을 때
                        Toast toast = Toast.makeText(MainActivity.this, "로그인 되었습니다.", Toast.LENGTH_SHORT );
                        toast.show();
						Intent intent = new Intent(MainActivity.this, showactivity.class);
						intent.putExtra("value",_id);// pass the value(_id)입력한 아이디값을 다음 액티비티로 넘겨줌
						startActivity(intent);// go to showactivity
                }
                else if (flag2 == 0) {// enter id != stored id입력한 아이디 값이 없을 경우
                    Toast toast = Toast.makeText(MainActivity.this, "다시 로그인해주세요", Toast.LENGTH_SHORT );
                    toast.show();
                }
                flag2 = 0;//reset flag 다시 돌려놓기
			}
		});

	}

	public String get_id(){//editText value to string
		return _id;
	}

	private String SendByHttp(String msg, String address_id) {
		if(msg == null)
			msg = "";

		String URL = "";

			URL = "http://52.78.209.143/users/"+address_id;//base URL with enter id = user URL

		DefaultHttpClient client = new DefaultHttpClient();
		try {
			HttpGet get = new HttpGet(URL+"?msg="+msg);// Send to id server to check

			HttpParams params = client.getParams();
			HttpConnectionParams.setConnectionTimeout(params, 3000);
			HttpConnectionParams.setSoTimeout(params, 3000);// Delay time up to 3 seconds

			HttpResponse response = client.execute(get);
			BufferedReader bufreader = new BufferedReader(
					new InputStreamReader(response.getEntity().getContent(),// The process of receiving data from the server
							"utf-8"));

			String line = null;
			String result = "";

			while ((line = bufreader.readLine()) != null) {
				result += line;
			}

			return result;
		} catch (Exception e) {
			e.printStackTrace();
			client.getConnectionManager().shutdown();	// Connection delay termination
			return "";
		}
	}

	void jsonParserList1(String pRecvServerPage){
		StringBuffer sb = new StringBuffer();

		try {// Analyze received values

			JSONArray jarray = new JSONArray(pRecvServerPage);

			for(int i=0; i < jarray.length(); i++) {

				JSONObject jObject = jarray.getJSONObject(i);

				int count = jObject.getInt("count");
				int id = jObject.getInt("id");

                if(int_id == id)//when enterid == stored id >> change flag2 1
                    flag2 = 1;

				String name = jObject.getString("name");

				sb.append(
						"아이디:" + id + "\n" +
								"이름:" + name + "\n" +
								"횟수:" + count + "\n"
				);
			}
		} catch (JSONException e) {
			e.printStackTrace();
		}
	}
}

