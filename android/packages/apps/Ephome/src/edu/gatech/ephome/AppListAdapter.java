package edu.gatech.ephome;

import android.annotation.TargetApi;
import android.content.Context;
import android.os.Build;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;
import android.widget.ImageView;
import android.widget.TextView;

import com.android.volley.toolbox.NetworkImageView;

import java.util.ArrayList;
import java.util.Collection;

public class AppListAdapter extends ArrayAdapter<AppModel> {
    private final LayoutInflater mInflater;
    final String DTAG = "EPHOME_APPLISTADAPTER";


    public AppListAdapter (Context context) {
        super(context, android.R.layout.simple_list_item_2);

        mInflater = LayoutInflater.from(context);
    }

    public void setData(ArrayList<AppModel> data) {
        clear();
        if (data != null) {
            addAll(data);
        }
    }

    /**
     * Populate new items in the list.
     */
    @Override public View getView(int position, View convertView, ViewGroup parent) {
        View view;

        if (convertView == null) {
            view = mInflater.inflate(R.layout.list_item_icon_text, parent, false);
        } else {
            view = convertView;
        }

        AppModel item = getItem(position);
        ((NetworkImageView)view.findViewById(R.id.icon)).setImageUrl(item.getIconURL(),item.getImageLoader());
        ((TextView)view.findViewById(R.id.text)).setText(item.getLabel());
	Log.d("EPHOME_PERF", "itime: " + System.nanoTime());

        return view;
    }
}
