#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <cutils/sockets.h>
#include <cutils/log.h>
#include <ephemeralutils/ephemeral_client.h>
#include <ephemeralutils/ephemeral_log.h>
/* log information. used to monitor the flow of Strapp debug logs.*/
#define LOG_EPHEMERAL_ERROR_TAG            "Ephemeral_cliE"
#define LOG_EPHEMERAL_INFO_TAG             "Ephemeral_cliI" 

#define LOGPI(...)    EPHLOGI(LOG_EPHEMERAL_INFO_TAG, __VA_ARGS__)
#define LOGCLI(...)    EPHLOGI(LOG_EPHEMERAL_INFO_TAG, __VA_ARGS__)
#define LOGPE(...)    EPHLOGE(LOG_EPHEMERAL_ERROR_TAG, __VA_ARGS__)


char* app_name_list[2] = {
"com.adobe.reader",
"com.nytimes.android"
};

char* asset_name_list[2] = {
"AndroidManifest.xml",
"res/drawable/dummy.png"
};

int basic_test()
{
	// case 1: simple 1-log, 1-truncate and clear
	int res = 0;
	LOGPI(">>> basic_test\n");
	res = ephemeral_log(app_name_list[0],STATIC_ASSET,asset_name_list[0]);
	if(res < 0)
	{
		LOGPE("Issue logging");
		return -1;
	}
	
	res = ephemeral_truncate(app_name_list[0]);
	if(res < 0)
	{
		LOGPE("Issue Truncating");
		return -2;
	}

	res = ephemeral_clearlog(app_name_list[0]);
	if(res < 0)
	{
		LOGPE("Issue Clearing");
		return -3;
	}
	LOGPI("<<< basic_test\n");
	return 0;
}

int basic_test2()
{
	int res = 0;
        LOGPI(">>> basic_test2\n");
        res = ephemeral_log(app_name_list[0],STATIC_ASSET,asset_name_list[0]);
        if(res < 0)
        {
                LOGPE("Issue logging");
                return -1;
        }
        res = ephemeral_log(app_name_list[0],STATIC_ASSET,asset_name_list[1]);
        if(res < 0)
        {
                LOGPE("Issue logging");
                return -1;
        }
        
        res = ephemeral_truncate(app_name_list[0]);
        if(res < 0)
        {
                LOGPE("Issue Truncating");
                return -2;
        }

        res = ephemeral_clearlog(app_name_list[0]);
        if(res < 0)
        {
                LOGPE("Issue Clearing");
                return -3;
        }
        LOGPI("<<< basic_test2\n");
        return 0;

}

int basic_test3()
{
        int res = 0;
        LOGPI(">>> basic_test3\n");
        res = ephemeral_log(app_name_list[0],STATIC_ASSET,asset_name_list[0]);
        if(res < 0)
        {
                LOGPE("Issue logging");
                return -1;
        }
        res = ephemeral_log(app_name_list[0],STATIC_ASSET,asset_name_list[1]);
        if(res < 0)
        {
                LOGPE("Issue logging");
                return -1;
        }
        res = ephemeral_log(app_name_list[1],STATIC_ASSET,asset_name_list[0]);
        if(res < 0)
        {
                LOGPE("Issue logging");
                return -1;
        }
        res = ephemeral_log(app_name_list[1],STATIC_ASSET,asset_name_list[1]);
        if(res < 0)
        {
                LOGPE("Issue logging");
                return -1;
        }
        
        res = ephemeral_truncate(app_name_list[0]);
        if(res < 0)
        {       
                LOGPE("Issue Truncating");
                return -2;
        }
        
	res = ephemeral_truncate(app_name_list[1]);
        if(res < 0)
        {       
                LOGPE("Issue Truncating");
                return -2;
        }

        res = ephemeral_clearlog(app_name_list[0]);
        if(res < 0)
        {
                LOGPE("Issue Clearing");
                return -3;
        }
        res = ephemeral_clearlog(app_name_list[1]);
        if(res < 0)
        {
                LOGPE("Issue Clearing");
                return -3;
        }
        LOGPI("<<< basic_test3\n");
        return 0;
}

int basic_testil()
{

	int res = 0;
        LOGPI(">>> basic_testil\n");
        res = ephemeral_log(app_name_list[0],STATIC_ASSET,asset_name_list[0]);
        if(res < 0)
        {
                LOGPE("Issue logging");
                return -1;
        }
        res = ephemeral_log(app_name_list[1],STATIC_ASSET,asset_name_list[0]);
        if(res < 0)
        {
                LOGPE("Issue logging");
                return -1;
        }
        res = ephemeral_log(app_name_list[0],STATIC_ASSET,asset_name_list[1]);
        if(res < 0)
        {
                LOGPE("Issue logging");
                return -1;
        }
        res = ephemeral_log(app_name_list[1],STATIC_ASSET,asset_name_list[1]);
        if(res < 0)
        {
                LOGPE("Issue logging");
                return -1;
        }

        res = ephemeral_truncate(app_name_list[1]);
        if(res < 0)
        {
                LOGPE("Issue Truncating");
                return -2;
        }

        res = ephemeral_truncate(app_name_list[0]);
        if(res < 0)
        {
                LOGPE("Issue Truncating");
                return -2;
        }

	res = ephemeral_clearlog(app_name_list[0]);
        if(res < 0)
        {
                LOGPE("Issue Clearing");
                return -3;
        }
        res = ephemeral_clearlog(app_name_list[1]);
        if(res < 0)
        {
                LOGPE("Issue Clearing");
                return -3;
        }
        LOGPI("<<< basic_testil\n");
        return 0;


}


int main()
{
	int res = 0;
	res = basic_test();
	if(res < 0)
	{
		LOGPE("1 - Failed");
		return res;
	}
	
	res = basic_test2();
	if(res < 0)
	{
		LOGPE("2 - Failed");
		return res;
	}

	res = basic_test3();
	if(res < 0)
	{
		LOGPE("3 - Failed");
		return res;
	}
	res = basic_testil();
	if(res < 0)
	{
		LOGPE("IL - Failed");
		return res;
	}
	return 0;
	
}

#if 0
char* cl_name_list[2] = {
"com.google.android.apps.maps.apk.classes}",
"com.amazon.pricecheck.apk.classes}",
"com.dropbox.android.apk.classes}",
"com.twitter.android.apk.classes}",
"com.evernote.apk.classes}",
"com.outfit7.talkingtom.apk.classes}",
"br.com.rodrigokolb.realguitar.apk.classes}",
"br.com.rodrigokolb.realpiano.apk.classes}",
"com.linkedin.android.apk.classes}",
"com.saavn.android.apk.classes}",
"com.accuweather.android.apk.classes}",
"com.yelp.android.apk.classes}",
"com.yellowpages.android.gas.apk.classes}"
};
int main()
{

//	char *class_name = "com.strapp.let.see";
	char *class_buf = NULL;
	int class_size = 0;
	int i = 0, j = 0;
	int sockfd = -1;
	ssize_t var;
	LOGPI("size of int: %ld\n",sizeof(int));
	sockfd = socket_network_client("130.207.5.67", 3000, SOCK_STREAM);
	if(sockfd == -1)
	{

		LOGPE("Issue with opening a socket\n");
//		return -3;
	}
	for(j=0;j<1;j++)
	{
		for(i=0;i<2;i++)
		{
			class_size = ephemeral_get(sockfd,JCLASS,cl_name_list[i], (void**)&class_buf);
			LOGPI("Size of class: %d\n", class_size);
			if(class_size < 0)
			{
				LOGPI("Nothing recieved from cell\n");
			break;
			}
			else
			{
			LOGPI("Class magic: %x %x %x %x \n",class_buf[0],class_buf[1],class_buf[2],class_buf[3]);
			}

			// Freeing of the class buffer needs to be taken care by the requested i.e. Dalvik VM
			if(class_buf != NULL) 
			{
				free(class_buf);
				LOGPI("Free class buffer\n");
				class_buf = NULL;
			}
		LOGPI("Iteration %d -------------- \n",i);
		}
	}

	socket_network_cleanup(sockfd);

	return 0;

}
#endif

