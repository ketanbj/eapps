package edu.gatech.ephome;


import android.content.Context;
import android.content.pm.PackageManager;

import android.graphics.Bitmap;
import android.graphics.drawable.BitmapDrawable;
import android.graphics.drawable.Drawable;
// import android.support.v4.content.Loader;
import android.content.Loader;
import android.util.Log;

import java.text.Collator;
import java.util.ArrayList;
import java.util.Comparator;
import java.util.concurrent.ExecutionException;

import com.android.volley.Request;
import com.android.volley.RequestQueue;
import com.android.volley.Response;
import com.android.volley.VolleyError;
import com.android.volley.toolbox.ImageRequest;
import com.android.volley.toolbox.JsonObjectRequest;
import com.android.volley.toolbox.RequestFuture;
import com.android.volley.toolbox.Volley;

import org.json.*;

public class AppsLoader extends Loader<ArrayList<AppModel>> {
    ArrayList<AppModel> meApps;

    final String DTAG = "EPHOME_APPSLOADER";
    Context mContext = null;
    ArrayList<AppModel> items = null;

    final PackageManager mPm;
    //TODO: PackageIntentReceiver mPackageObserver;
    AppsNwMonitor mNetworkMonitor;

    RequestQueue mEpPackageManager = null;

    public AppsLoader(Context context) {
        super(context);
        mContext = context;
        mPm = context.getPackageManager();
    }

    @Override
    protected void onForceLoad()
    {
        Log.d(DTAG,"AppsLoader -- onForceLoad");

        if (mNetworkMonitor == null) {
            mNetworkMonitor = new AppsNwMonitor(this);
         }

        if(mEpPackageManager == null)
            mEpPackageManager = Volley.newRequestQueue(mContext);

        // Remove all elements from items list
        if(items!= null && items.size() != 0)
            items.clear();
        // Request apps again
        if(mNetworkMonitor.isConnected())
            request_eapps();
        else
        {
            deliverResult(null);
        }
    }

    @Override
    protected void onReset()
    {
        Log.d(DTAG,"AppsLoader -- onReset");
    }

    @Override
    protected void onStartLoading()
    {
        Log.d(DTAG,"AppsLoader -- onStartLoading");

        if (mNetworkMonitor == null) {
            mNetworkMonitor = new AppsNwMonitor(this);
        }
        if(mEpPackageManager == null)
            mEpPackageManager = Volley.newRequestQueue(mContext);

        if(mNetworkMonitor.isConnected())
            request_eapps();
        else
        {
            deliverResult(null);
        }

    }

    @Override
    protected void onStopLoading() {
        // Attempt to cancel the current load task if possible.
        Log.d(DTAG,"AppsLoader -- onStopLoading");
        /* Stop monitoring for changes.*/
        if (mNetworkMonitor != null) {
            getContext().unregisterReceiver(mNetworkMonitor);
            mNetworkMonitor = null;
        }
    }

   /**
     * Perform alphabetical comparison of application entry objects.
     */
    public static final Comparator<AppModel> ALPHA_COMPARATOR = new Comparator<AppModel>() {
        private final Collator sCollator = Collator.getInstance();
        @Override
        public int compare(AppModel object1, AppModel object2) {
            return sCollator.compare(object1.getLabel(), object2.getLabel());
        }
    };

    public void parse_apps(JSONObject response)
    {
        //TODO: parse from response

        JSONArray eApps = null;
        int nbApps = 0;

        try {
            nbApps = response.getInt("length");
            eApps = response.getJSONArray("apps");

            items = new ArrayList<AppModel>(nbApps);

            if(nbApps != 0 && eApps.length() > 0) {
                for (int i = 0; i < eApps.length(); i++) {
                    // Get app label
                    String label = eApps.getJSONObject(i).getString("label");
                    Log.d(DTAG, "App label:"+ eApps.getJSONObject(i).getString("label"));
                    // Get app's icon url
                    String icon_url = eApps.getJSONObject(i).getString("icon_url");
                    Log.d(DTAG, "App icon_url:"+ eApps.getJSONObject(i).getString("icon_url"));

                    String pkg = eApps.getJSONObject(i).getString("package");
                    Log.d(DTAG, "App package name:"+ eApps.getJSONObject(i).getString("package"));

                    String activity = eApps.getJSONObject(i).getString("activity");
                    Log.d(DTAG, "App activity:"+ eApps.getJSONObject(i).getString("activity"));

                    // add to list of apps to be delivered
                    AppModel item  = new AppModel(mContext,mEpPackageManager,label,icon_url,pkg,activity);
                    items.add(item);
                }
            }
        }
        catch(Exception e)
        {
            Log.d(DTAG,"Error in json response from server" + e);
        }
	Log.d("EPHOME_PERF", "ptime: " + System.nanoTime());
        deliverResult(items);

    }

    public void request_eapps()
    {
        String murl = "http://54.145.31.254:3000/eapps";

        Log.d(DTAG, "Fetching ephemeral apps from URL: " + murl);
        // Request a string response from the provided URL.
        JsonObjectRequest jsonreq = new JsonObjectRequest(Request.Method.GET, murl,null,
                new Response.Listener<JSONObject>() {
                    @Override
                    public void onResponse(JSONObject response) {
                        // Display the response string.
                        Log.d(DTAG, "Get Response is: " + response.toString());
			Log.d("EPHOME_PERF", "gtime: " + System.nanoTime());
                        //TODO: Create application info from response
                        parse_apps(response);
                    }
                }, new Response.ErrorListener() {
            @Override
            public void onErrorResponse(VolleyError error) {
                Log.d(DTAG,"Get json didn't work - " + error);
            }
        });
        // Add the request to the RequestQueue.
        mEpPackageManager.add(jsonreq);

    }

}
