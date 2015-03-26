package edu.gatech.ephome;

import android.content.Context;
import android.content.pm.ApplicationInfo;
import android.graphics.Bitmap;
import android.graphics.drawable.BitmapDrawable;
import android.graphics.drawable.Drawable;
// import android.support.v4.util.LruCache;
import android.util.LruCache;
import android.util.Log;
import android.content.pm.ApplicationInfo;
import com.android.volley.RequestQueue;
import com.android.volley.Response;
import com.android.volley.VolleyError;
import com.android.volley.toolbox.ImageLoader;
import com.android.volley.toolbox.ImageRequest;

import java.io.File;

public class AppModel {

    private final Context mContext;
    private final ApplicationInfo mInfo;
    final String DTAG = "EPHOME_APPMODEL";

    private String mAppLabel;
    private String mAppIconUrl;
    private ImageLoader mImageLoader;
    private String mPackageName;
    private String mActivityName;
    private ApplicationInfo mApplicationInfo;
    //private boolean mMounted;
    //private final File mApkFile;

    public AppModel(Context context, RequestQueue q, String label, String iconUrl,String pkge, String activity) {
        mContext = context;
        mAppLabel = label;
        mAppIconUrl = iconUrl;
        mImageLoader = new ImageLoader(q,
                new ImageLoader.ImageCache() {
                    private final LruCache<String, Bitmap>
                            cache = new LruCache<String, Bitmap>(20);

                    @Override
                    public Bitmap getBitmap(String url) {
                        return cache.get(url);
                    }

                    @Override
                    public void putBitmap(String url, Bitmap bitmap) {
                        cache.put(url, bitmap);
                    }
                });
        mInfo = new ApplicationInfo();
	mInfo.processName = "ephemeral-app";
	mInfo.className = activity;
	mInfo.theme = 0;
	mInfo.manageSpaceActivityName = null; //FIXME: needs to come from manifest.xml
	mInfo.backupAgentName = null;
	mInfo.primaryCpuAbi = "armeabi-v7a";
	mInfo.uid = 18999;
    }

    public ApplicationInfo getAppInfo() {
        return mInfo;
    }

    public String getApplicationPackageName() {
        return mPackageName;
    }

    public String getLabel() {
        return mAppLabel;
    }
    public String getIconURL()
    {
        return mAppIconUrl;
    }

    public ImageLoader getImageLoader()
    {
        return mImageLoader;
    }

    public ApplicationInfo getApplicationInfo()
    {
	return mApplicationInfo;
    }

}
