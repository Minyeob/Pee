package com.example.androidsendreceivetest;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.view.Window;
import android.widget.ImageView;

import com.github.mikephil.charting.charts.BarChart;
import com.github.mikephil.charting.components.YAxis;
import com.github.mikephil.charting.data.BarData;
import com.github.mikephil.charting.data.BarDataSet;
import com.github.mikephil.charting.data.BarEntry;

import java.util.ArrayList;
import java.util.StringTokenizer;

/**
 * Created by jeonjinwoo on 2016-12-06.
 */

public class chartactivity extends Activity {

    ImageView iv;
    BarChart chart;
    ArrayList<BarEntry> entries;
    String[] Yvalue = new String[100];// All Y values
    String[] Xvalue = new String[100];// All X values
    int counting = 0;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        requestWindowFeature(Window.FEATURE_NO_TITLE);// Remove top titlebar
        setContentView(R.layout.chart_activity);

        iv = (ImageView)findViewById(R.id.chart_image);
        chart = (BarChart) findViewById(R.id.charz);
        entries = new ArrayList<BarEntry>();

        Intent intent2 = getIntent();
        String[] data_time = intent2.getStringArrayExtra("new_data");// get the value from showactivity

        for(int p = 0; p<data_time.length; p++){

            if(data_time[p] != null){
                counting++;// find last index
            }
        }

        for(int k = 0; k < counting; k++){

            StringTokenizer st = new StringTokenizer(data_time[k],"^");

                Yvalue[k] = st.nextToken("^");// Yvals
                Xvalue[k] = st.nextToken("^");// Xvals
        }
        for(int i = 0; i < counting; i++){// Enter chart values
            entries.add(new BarEntry(Float.parseFloat(Yvalue[i]),i));
        }

        BarDataSet setComp1 = new BarDataSet(entries, "USER");
        setComp1.setAxisDependency(YAxis.AxisDependency.LEFT);

        ArrayList<BarDataSet> dataSets = new ArrayList<BarDataSet>();
        dataSets.add(setComp1);

        ArrayList<String> xVals = new ArrayList<String>();

        for(int t = 1; t<=counting; t++){// add xVals
            xVals.add("");
        }
        chart.setDrawGridBackground(false);// delete background
        chart.setDrawBarShadow(false);// delete Bar
        chart.getAxisRight().setDrawLabels(false);// delete Axis

        BarData data = new BarData(xVals, dataSets);
        chart.setData(data);
        chart.invalidate();
    }
}
