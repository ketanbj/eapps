#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <cutils/sockets.h>
#include <cutils/log.h>
#include <ephemeralutils/ephemeral_client.h>
/* log information. used to monitor the flow of Strapp debug logs.*/
#define LOG_EPHEMERAL_ERROR_TAG            "Ephemeral_E"
#define LOG_EPHEMERAL_INFO_TAG             "Ephemeral_I" 

#define LOGPI(...)    EPHLOGI(LOG_EPHEMERAL_INFO_TAG, __VA_ARGS__)
#define LOGCLI(...)    EPHLOGI(LOG_EPHEMERAL_INFO_TAG, __VA_ARGS__)
#define LOGPE(...)    EPHLOGE(LOG_EPHEMERAL_ERROR_TAG, __VA_ARGS__)


#if 0
/* Connect to port on the IP interface. type is
 * SOCK_STREAM or SOCK_DGRAM.
 * return is a file descriptor or -1 on error
 */
int socket_network_client(const char *host, int port, int type)
{
    struct hostent *hp;
    struct sockaddr_in addr;
    socklen_t alen;
    int s;

    hp = gethostbyname(host);
    if(hp == 0) return -1;

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = hp->h_addrtype;
    addr.sin_port = htons(port);
    memcpy(&addr.sin_addr, hp->h_addr, hp->h_length);

    s = socket(hp->h_addrtype, type, 0);
    if(s < 0) return -1;

    if(connect(s, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
        close(s);
        return -1;
    }

    return s;

}

int socket_network_cleanup(int fd)
{
	close(fd);
	return 0;
}

// These are specifically chosen from the hex codes 
// which are not used in DEX byte code format

// strapp streaming protocol codes
#define EOC 0xFC
#define ACK 0xFF

// strapp streaming error codes
#define CNS 0xE3
#define ESC 0xE4

typedef struct _strapp_stream
{
	int ack;
	int eoc;
	int cns;
	int esc;
	unsigned char *byte_code;
	unsigned int size_code;
	unsigned int code_rec;
}strapp_stream;


void dump_strapp_status(strapp_stream * ptr)
{
	printf("ACK: %d EOC: %d ESC: %d CNS: %d CS: %d RC: %d\n",ptr->ack, ptr->eoc,ptr->esc, ptr->cns, ptr->size_code, ptr->code_rec );
}	

int parse(unsigned char *bufTemp, strapp_stream *strapp_pkt, ssize_t numBytes)
{
	int res = -1;
	int readBytes = 0, remBytes = 0, classBytes = 0,countBytesCopied = 0;
	unsigned char *ptr = bufTemp, *cp_ptr;
	unsigned int *sizeptr;
	
	//printf("bufTemp magic: %x %x %x %x %x %x\n",bufTemp[0],bufTemp[1],bufTemp[2],bufTemp[3],bufTemp[4],bufTemp[5]);
	while(readBytes < numBytes)
	{
	//	printf("*ptr = %X %d %c readBytes: %d numBytes: %d\n",*ptr,*ptr,*ptr ,readBytes, numBytes);
		countBytesCopied = 0;
		remBytes = 0;
		if(!strapp_pkt->ack && *ptr == ACK)
		{
			printf("ACK\n");
			strapp_pkt->ack = 1;
			readBytes++;
			res = 0;
			ptr++;
			continue;
		}
		else if(strapp_pkt->ack && strapp_pkt->size_code == 0)
		{
			printf("size: %x %x %x %x\n",ptr[0],ptr[1],ptr[2],ptr[3]);
			sizeptr = (unsigned int*)ptr;
			strapp_pkt->size_code = *sizeptr;		
			printf("Got expected size: %d\n",*sizeptr);
			ptr+=sizeof(int);
			readBytes+=sizeof(int);
			res = 0;
			continue;
		}
		else if(strapp_pkt->ack && strapp_pkt->size_code == 0 &&  *ptr == CNS)
		{
			printf("CNS\n");
			strapp_pkt->cns = 1;
			readBytes++;
			res = 0;
			break;
		}
		/*else if(strapp_pkt->ack && *ptr == ESC)
		{
			printf("ESC\n");
			strapp_pkt->esc = 1;
			readBytes++;
			res = 0;
			ptr++;
			break;
		}*/
		else if(strapp_pkt->ack && !strapp_pkt->eoc)
		{
			// we are getting the class byte code
			// save it in buffer and return
			remBytes = numBytes-readBytes;
			strapp_pkt->byte_code = realloc(strapp_pkt->byte_code,(strapp_pkt->code_rec+remBytes)*sizeof(char));
			if(!strapp_pkt->byte_code)
			{
				printf("Not enough memory\n");
				return -1;
			}
			
			//memcpy(strapp_pkt->byte_code+strapp_pkt->size_code, bufTemp, numBytes-readBytes);
			cp_ptr = strapp_pkt->byte_code+strapp_pkt->code_rec;
			//parsedBytes = numBytes - readBytes;
			while(countBytesCopied < remBytes)
			{
				//printf("Copying byte by byte \n");
				*cp_ptr++ = *ptr++;
				countBytesCopied++;
			}
			
	
			strapp_pkt->code_rec+=countBytesCopied;
			readBytes+=countBytesCopied;

		//	printf("Parsed size: %d Bytes - Expected size: %d \n",strapp_pkt->code_rec,strapp_pkt->size_code);

//			printf("P--");
//			dump_strapp_status(strapp_pkt);	
			if(strapp_pkt->code_rec == strapp_pkt->size_code)
			{
				printf("EOC\n");
				strapp_pkt->eoc = 1;
                                //strapp_pkt->ack = 0;
				//res = 0
				//break;
				
			}
			res = 0;
		}
		else
		{
			printf("Unexpected bytes\n");
			res = -1;
			break;
		}
		
		//if(strapp_pkt->eoc) break;
		
	}
	//printf(" Parse - readBytes: %d numBytes: %d\n", readBytes, numBytes);
	return res;
}


int get_class_from_network(int sockfd, const char * class_name, void** clBuf)
{

	unsigned char bufOut[4*16*1024];
	unsigned char bufTemp[4*16*1024];
	int *bufIn = NULL;
	ssize_t bytesWritten = 0, bytesRead = 0, numBytes = 0;
	strapp_stream strapp_pkt;
	int res = -1;
//	printf("Hello -- starting a socket client\n");
	
	if(class_name == NULL)
	{
		printf("Bad Class name\n");
		return -1;

	}

	if(sockfd == -1)
	{
		printf("Bad socket\n");
		return -1;
	}
	
	memcpy(&bufOut[0],"0",4*16*1024);	
	strcpy(bufOut,class_name);
	
	bytesWritten = write(sockfd,bufOut, strlen(bufOut));

	if(bytesWritten < strlen(bufOut))
	{
		printf("write returned bytesWritten: 	%zu\n", bytesWritten);
		//return -2;
	}

	printf("Request --> %s\n",bufOut);

	memset(&strapp_pkt,0,sizeof(strapp_stream));

	while(!strapp_pkt.eoc)
	{
		memset(bufTemp, 0,4*16*1024*sizeof(char));

//		printf("B--");
//		dump_strapp_status(&strapp_pkt);	

		numBytes = read(sockfd,bufTemp,sizeof(bufTemp));
		printf("Recieved %zu Byte(s) <--\n",numBytes);

		res = parse(bufTemp,&strapp_pkt, numBytes);
		if(res == -1 ||  strapp_pkt.cns || strapp_pkt.esc)
		{
			printf("Error Parsing stream\n");
			return -1;
		}

//		printf("A--");
//		dump_strapp_status(&strapp_pkt);	
		printf("Waiting for class data ... \n");
	}

	printf("Class data recieve complete\n");
	printf("magic: %x %x %x %x \n",strapp_pkt.byte_code[0],strapp_pkt.byte_code[1],strapp_pkt.byte_code[2],strapp_pkt.byte_code[3]); 
	
	// copy buffer in buffer provided
	//memcpy(clBuf, bufIn,bytesRead);
	*clBuf = strapp_pkt.byte_code;

	return strapp_pkt.size_code;	
}
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

#endif



char* cl_name_list[15] = {
"com.adobe.reader.apk.classes}",
"com.nytimes.android.apk.classes}"
};

#if 1
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

