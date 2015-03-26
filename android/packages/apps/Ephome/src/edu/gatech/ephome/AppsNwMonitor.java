package edu.gatech.ephome;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.net.ConnectivityManager;
import android.net.NetworkInfo;
import android.util.Log;

/**
 * Helper class to look for interesting changes to the installed apps
 * so that the loader can be updated.
 *
 * @Credit http://developer.android.com/reference/android/content/AsyncTaskLoader.html
 */
public class AppsNwMonitor extends BroadcastReceiver {

    final AppsLoader mLoader;
    final String DTAG = "EPHOME_NWMONITOR";

    public AppsNwMonitor(AppsLoader loader) {
        mLoader = loader;

        IntentFilter filter = new IntentFilter(android.net.ConnectivityManager.CONNECTIVITY_ACTION);
        filter.addAction(android.net.ConnectivityManager.CONNECTIVITY_ACTION);
        //filter.addDataScheme("package");
        mLoader.getContext().registerReceiver(this, filter);
        Log.d(DTAG, "Registered Network State change Listener");

    }

    public boolean isConnected()
    {
        ConnectivityManager cm =
                (ConnectivityManager)mLoader.mContext.getSystemService(Context.CONNECTIVITY_SERVICE);

        NetworkInfo activeNetwork = cm.getActiveNetworkInfo();

        boolean isConnected = activeNetwork != null &&
                activeNetwork.isConnectedOrConnecting();
        Log.d(DTAG,"Network State change Listener - isConnected "+isConnected);
        return isConnected;

    }

    @Override public void onReceive(Context context, Intent intent) {
        // Tell the loader about the change.
        Log.d(DTAG,"Network State change - OnReceive");
        ConnectivityManager cm =
                (ConnectivityManager)context.getSystemService(Context.CONNECTIVITY_SERVICE);

        NetworkInfo activeNetwork = cm.getActiveNetworkInfo();

        if(activeNetwork != null) {
            boolean isConnected = activeNetwork.isConnectedOrConnecting();
            if (isConnected) {
                Log.d(DTAG, "Nw type: " + activeNetwork.getTypeName() + "(" + activeNetwork.getType() + ")");
                String extraInfo = activeNetwork.getExtraInfo();
                if (extraInfo != null)
                    Log.d(DTAG, "Extra Info: " + extraInfo);
            }
        }
        else
        {

        }

        mLoader.onContentChanged();
    }

}
