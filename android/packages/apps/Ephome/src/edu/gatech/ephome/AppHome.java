package edu.gatech.ephome;

import android.accounts.Account;
import android.accounts.AccountManager;
import android.accounts.AccountManagerFuture;
import android.accounts.AuthenticatorException;
import android.accounts.OperationCanceledException;
import android.app.Activity;
import android.database.Cursor;
import android.net.Uri;
import android.os.Bundle;
// import android.support.v4.app.FragmentActivity;
// import android.app.FragmentActivity;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;

import java.io.IOException;


public class AppHome extends Activity {

    final String LOG_TAG = "EPHOME";

   /* @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.menu_app_home, menu);
        Log.d(LOG_TAG, "OnCreateOptionsMenu");
        return true;
    }*/

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_app_home);
        Log.d(LOG_TAG, "OnCreate");

	if (savedInstanceState == null) {
            getFragmentManager()
                    .beginTransaction()
                    .add(R.id.apps_grid, new AppsGridFragment())
                    .commit();
        }
	Log.d("EPHOME_PERF","stime: "+ System.nanoTime());
    }
}
