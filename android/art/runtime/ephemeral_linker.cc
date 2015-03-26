#include "ephemeral_linker.h"

#include <algorithm>
#include <string>
#include <memory>


#include "cutils/sockets.h"
#include "ephemeralutils/ephemeral_client.h"
#include "base/logging.h"
#include "mem_map.h"

namespace art {

EphemeralLinker::EphemeralLinker():sockfd(-1){
}

bool EphemeralLinker::ReadEphemeralApps()
{
     FILE* fp = fopen("/data/ephemeralapps/app-list", "r");
     if (fp == NULL) {
        LOG(ERROR) << "Unable to open strapp config";
         return false;
     }

     char f_line[500];
     while(fgets(f_line,500,fp) != NULL)
     {
        char* cur_line = strtok(f_line," \t\r\n");
	apps.push_back(std::string(cur_line, strlen(cur_line)));

     }
     LOG(ERROR) << "Added" << apps.size() <<"Ephemeral apps from app-list";
     return true;

}

bool EphemeralLinker::OpenSocket()
{
	bool res = false;
	if(sockfd == -1)
        {
		FILE* fp = fopen("/data/ephemeralapps/ip-list", "r");
     		if (fp == NULL) 
		{
        		LOG(ERROR) << "Unable to open strapp-ip config file";
			return false;
     		}

		char fLine[500];
     		while(fgets(fLine,500,fp) != NULL)
     		{
        		char* curLine = strtok(fLine," \t\r\n");
			sockfd = socket_network_client(curLine, 3000, SOCK_STREAM);
			if(sockfd > 0)
			{
				LOG(INFO) << "Using strapp IP: " << curLine;
				res  = true;
				break;
			}
		}
        }
       return res;
}


bool EphemeralLinker::FetchDex(const char* sourceName, unsigned char **ppNwBytes,int* pNwLength)
{
        VLOG(ephemeral_linker) << "fetch_exe Entry !";
	//RawDexFile* pRawDexFile;
        /* extract package name from installed name */
        if(apps.empty())
        {
		bool check = EphemeralLinker::ReadEphemeralApps();
		if(!check)
		{
                	LOG(WARNING) << "No available ephemeral apps";
			return false;
		}
        }
    
    
	LOG(INFO) << "# available ephemeral apps: " << apps.size();
        char * pch = NULL,*last = NULL, *reqName = NULL, *secondlast = NULL;
        char * pkgName = strdup(sourceName);
        pch = strtok(pkgName,"/");
        // removing directory name
        while (pch != NULL)
        {
        	//FIXME: if(last != NULL) free(last);
		if(last != NULL) secondlast = strdup(last);
                last = strdup(pch);
                pch = strtok(NULL, "/");
        }
	//LOG(INFO) << "last: " << last <<" secondlast: " << secondlast ;

        if(strstr(secondlast,"-"))
        {
        	pch = strtok(secondlast,"-");
                reqName = (char*) malloc(strlen(pch)+strlen(".apk}")+1);
                strcpy(reqName,pch);
                strcat(reqName,".apk");
        }
        else
        {
        	pch = last;
                reqName = (char*) malloc(strlen(pch)+1);
                strcpy(reqName,pch);
        }
	//LOG(INFO) << "ReqName Name (without classes):" << reqName;

        // FIXME: Free the memory in correct order
	if (!(std::find(apps.begin(), apps.end(), std::string(reqName)) != apps.end()))
	{
        	LOG(INFO) << "Nothing to do -- Not an ephemeral app";
		return false;
        }
                

        unsigned char *pNwBytes = NULL;
        unsigned int nwLength = 0;
        char *req = NULL;

        if(!OpenSocket())
	{
		LOG(INFO) << "Unable to open ephemeral socket\n";
                return false;
	}
                       
        req = (char*)malloc(strlen(reqName)+strlen(".classes}")+1);
        strcpy(req,reqName);
        strcat(req,".classes}");
	ephemeral_req_type syt = JCLASS;

        LOG(INFO) << "Requesting: " << req;
        //if(reqName != NULL) free(reqName);

        nwLength = ephemeral_get(sockfd,syt,req, (void**)&pNwBytes);
        if(nwLength <= 0)
        {
        	LOG(WARNING) <<"Nothing recieved ";
		return false;
         }
	*ppNwBytes = (unsigned char*) pNwBytes;
	*pNwLength = nwLength;
         LOG(INFO) << "classes.dex # bytes: " << nwLength;

        //TODO: Where to handle freeing of pNwBytes ?
    
	VLOG(ephemeral_linker) << "fetch_dex Exit !\n";
	return true;
}

bool EphemeralLinker::OpenEphemeralDexFiles(const char* dex_location, const char* oat_location,
                           std::vector<std::string>* error_msgs,
                           std::vector<const DexFile*>* dex_files)
{
	bool res = false;
	int dex_size = 0;
	unsigned char* dex_buffer;
	uint32_t location_checksum = 357;
	std::string error_msg;
	std::string location = std::string("ephemeral_",strlen("ephemeral_"))+ std::string(dex_location, strlen(dex_location));
	// Magic already checked in utisl on receiving classes.dex
        // FIXME: Check for a already opened OAT file containing this dex, should not happen in ephemeral app case though.
	// For ephemeral apps OAT file definitely is not on disk.
	//
	res = FetchDex(dex_location, &dex_buffer,&dex_size);
	if(!res || dex_size == 0)
	{
        	LOG(ERROR) << "Unable to receive Dex from ephemeral app server"<< dex_location;
		return false;
	}
#if 0
	MemMap* eph_mmap =  MemMap::MapAnonymous(location.c_str(), NULL, (size_t)dex_size,PROT_READ|PROT_WRITE,0, &error_msg);
	if(eph_mmap == nullptr)
	{
        	LOG(ERROR) << "Unable to create mmap";
		error_msgs->push_back(error_msg);
		return false;
		
	}
#endif
	// Open the dex file from memory
//	const DexFile* dex_file = DexFile::OpenEphemeralMemory((uint8_t*)dex_buffer, (size_t)dex_size, location, location_checksum,eph_mmap ,&error_msg);
	const DexFile* dex_file = DexFile::Open((uint8_t*)dex_buffer, (size_t)dex_size, location, location_checksum ,&error_msg);
	if(dex_file == nullptr)
	{
        	LOG(ERROR) << "Unable to create DexFile structure";
		error_msgs->push_back(error_msg);
		return false;	
	}

        LOG(INFO) << "Now using ephemeral dex";
	dex_files->push_back(dex_file);
	res = true;

	return res;
}


}//namespace
