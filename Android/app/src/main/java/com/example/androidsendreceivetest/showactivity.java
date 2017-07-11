package com.example.androidsendreceivetest;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.Window;
import android.widget.Button;
import android.widget.LinearLayout;
import android.widget.ListView;
import android.widget.SimpleAdapter;
import android.widget.TextView;

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
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.StringTokenizer;

public class showactivity extends Activity {

    ArrayList<String> items = new ArrayList<String>();

    SimpleAdapter adapter;
    ListView listview1;
    List<HashMap<String, String>> fillMaps;

    private Button btnSend,btnchart;
    private TextView tvRecvData;
            String address_id;
            LinearLayout headerr;
            int choice = 0;

            String[] new_data = new String[100];

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        requestWindowFeature(Window.FEATURE_NO_TITLE);// Remove top titlebar
        setContentView(R.layout.show_activity);

        listview1 = (ListView) findViewById(R.id.listview);
        headerr = (LinearLayout) findViewById(R.id.header);
        Intent intent = getIntent();
        String id = intent.getStringExtra("value");// get the value
        address_id = id;// User Account ID
        btnSend = (Button) findViewById(R.id.btn_sendData);
        tvRecvData = (TextView)	findViewById(R.id.tv_recvData);
        btnchart = (Button) findViewById(R.id.btn_chart);
        String result = SendByHttp(null, address_id);
        jsonParserList1(result);// JSON parsing

        String[] from = new String[] {"col_1", "col_2"};// key
        int[] to = new int[] { R.id.item1, R.id.item2};// value
        fillMaps = new ArrayList<HashMap<String, String>>();
        adapter = new SimpleAdapter(this, fillMaps, R.layout.grid_item, from, to);// connect key and value
        listview1.setAdapter(adapter);

        btnchart.setVisibility(View.INVISIBLE);// hide 'chart' button
        headerr.setVisibility(View.INVISIBLE);// hide listview header

        btnSend.setOnClickListener(new OnClickListener() {//show detail info(date & time)
            public void onClick(View v) {
                choice = 1;

                btnchart.setVisibility(View.VISIBLE);// when click the 'detail' button, show 'chart' button
                headerr.setVisibility(View.VISIBLE);// when click the 'detail' button, show listview header
                String result = SendByHttp(null, address_id);// send null to server and get the data
                jsonParserList2(result); // JSON parsing
            }
        });

        btnchart.setOnClickListener(new OnClickListener() {//show chart
            @Override
            public void onClick(View view) {

                Intent intent2 = new Intent(showactivity.this, chartactivity.class);
                intent2.putExtra("new_data",new_data);// pass value to chartactivity
                startActivity(intent2);// go to chartactivity
            }
        });
    }
    /*
     * @param msg
     * @return
     */
    private String SendByHttp(String msg, String address_id) {
        if(msg == null)
            msg = "";

        String URL = "";
        if(choice == 0){// when choice == 0, get user basic information
            URL = "http://52.78.209.143/users/"+address_id;
        }
        else if(choice == 1){// when choice == 1, get user detail information

            URL = "http://52.78.209.143/users/"+address_id+"/data";
        }
        DefaultHttpClient client = new DefaultHttpClient();
        try {
            HttpGet get = new HttpGet(URL+"?msg="+msg);
            HttpParams params = client.getParams();
            HttpConnectionParams.setConnectionTimeout(params, 3000);
            HttpConnectionParams.setSoTimeout(params, 3000);
            HttpResponse response = client.execute(get);
            BufferedReader bufreader = new BufferedReader(
                    new InputStreamReader(response.getEntity().getContent(),
                            "utf-8"));

            String line = null;
            String result = "";

            while ((line = bufreader.readLine()) != null) {
                result += line;
            }
            return result;
        } catch (Exception e) {
            e.printStackTrace();
            client.getConnectionManager().shutdown();
            return "";
        }
    }
    /*
     * @param
     * @return
     */

    void jsonParserList1(String pRecvServerPage){//json parsing
        StringBuffer sb = new StringBuffer();

        try {
            JSONArray jarray = new JSONArray(pRecvServerPage);

            for(int i=0; i < jarray.length(); i++) {

                JSONObject jObject = jarray.getJSONObject(i);
                Log.d("abc",">>>>"+jObject);
                int count = jObject.getInt("count");
                int id = jObject.getInt("id");
                String name = jObject.getString("name");

                sb.append(
                        "아이디 : " + id + "\n" +
                                "이름 : " + name + "\n" +
                                "횟수 : " + count + "\n"
                );
            }
            tvRecvData.setText(sb.toString());// show user basic information

        } catch (JSONException e) {
            e.printStackTrace();
        }
    }

    void jsonParserList2(String pRecvServerPage){
        StringBuffer ab = new StringBuffer();

        try {

            JSONArray jarray = new JSONArray(pRecvServerPage);
            int count=0;
            for(int i=0; i < jarray.length(); i++) {

                HashMap<String, String> map = new HashMap<String, String>();

                JSONObject jObject = jarray.getJSONObject(i);
                String date = jObject.getString("date");
                String time = jObject.getString("time");
                int id = jObject.getInt("user_id");
                Log.d("abc","@@@@@@@@@@@@"+date);

                ab.append(
                                "소요시간 : " + time + "   " +
                                "시각 : " + date + "\n"
                );
                StringTokenizer tokens = new StringTokenizer(date);

                String year = tokens.nextToken("T");// year value
                String date2 = tokens.nextToken(".");// day value
                date2 = date2.substring(1, date2.length());// delete "T"

                String etc = tokens.nextToken(".");// etc value

                map.put("col_1", time);// store time value to map
                map.put("col_2", date2);// store date2 value to map
                fillMaps.add(map);// add to fillmap
                new_data[count] = time+"^"+date2;// Merge time and date2 & pass to chartactivity

                count++;
            }
            adapter.notifyDataSetChanged();// renew the listview

        } catch (JSONException e) {
            e.printStackTrace();
        }
    }
}
