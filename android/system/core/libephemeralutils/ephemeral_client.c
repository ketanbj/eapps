#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <stddef.h>
#include <stdio.h>

#include <cutils/sockets.h>
#include <ephemeralutils/ephemeral_client.h>

int socket_network_cleanup(int fd)
{
        close(fd);
        return 0;
}


// These are specifically chosen from the hex codes
// which are not used in DEX byte code format

// ephemeral streaming protocol codes
#define EOC 0xFC
#define ACK 0xFF

// ephemeral streaming error codes
#define CNS 0xE3
#define ESC 0xE4

typedef struct _ephemeral_stream
{
        int ack;
        int eoc;
        int cns;
        int esc;
        unsigned char *byte_code;
        unsigned int size_code;
        unsigned int code_rec;
}ephemeral_stream;

void dump_status(ephemeral_stream * ptr)
{
        //printf("ACK: %d EOC: %d ESC: %d CNS: %d CS: %d RC: %d\n",ptr->ack, ptr->eoc,ptr->esc, ptr->cns, ptr->size_code, ptr->code_rec );
	ptr = ptr;
}


int parse(unsigned char *bufTemp, ephemeral_stream *ephemeral_pkt, ssize_t numBytes)
{
        int res = -1;
        int readBytes = 0, remBytes = 0, classBytes = 0,countBytesCopied = 0;
        unsigned char *ptr = bufTemp, *cp_ptr;
        unsigned int *sizeptr;

//        printf("bufTemp magic: %x %x %x %x %x %x\n",bufTemp[0],bufTemp[1],bufTemp[2],bufTemp[3],bufTemp[4],bufTemp[5]);
       //FIXME: Check recieved dex magic number here
//ubyte[8] DEX_FILE_MAGIC = { 0x64 0x65 0x78 0x0a 0x30 0x33 0x35 0x00 } = "dex\n035\0"
        while(readBytes < numBytes)
        {
              LOGPI("*ptr = %X %d %c readBytes: %d numBytes: %d\n",*ptr,*ptr,*ptr ,readBytes, numBytes);
                countBytesCopied = 0;
                remBytes = 0;
                if(!ephemeral_pkt->ack && *ptr == ACK)
                {
                        LOGPI("ACK\n");
                        ephemeral_pkt->ack = 1;
                        readBytes++;
                        res = 0;
                        ptr++;
                        continue;
                }
                else if(ephemeral_pkt->ack && ephemeral_pkt->size_code == 0 &&  *ptr == CNS)
                {
                        LOGPI("CNS\n");
                        ephemeral_pkt->cns = 1;
                        readBytes++;
                        res = 0;
                        break;

                        
                }
                else if(ephemeral_pkt->ack && ephemeral_pkt->cns != 1 && ephemeral_pkt->size_code == 0)
                {
			LOGPI("size: %x %x %x %x\n",ptr[0],ptr[1],ptr[2],ptr[3]);
                        sizeptr = (unsigned int*)ptr;
                        ephemeral_pkt->size_code = *sizeptr;
                        LOGPI("Got expected size: %d\n",*sizeptr);
                        ptr+=sizeof(int);
                        readBytes+=sizeof(int);
                        res = 0;
                        continue;
                }


                /*else if(ephemeral_pkt->ack && *ptr == ESC)
                {
                        printf("ESC\n");
                        ephemeral_pkt->esc = 1;
                        readBytes++;
                        res = 0;
                        ptr++;
                        break;
                }*/
		else if(ephemeral_pkt->ack && !ephemeral_pkt->eoc)
                {
                        // we are getting the class byte code
                        // save it in buffer and return
                        remBytes = numBytes-readBytes;
                        ephemeral_pkt->byte_code = realloc(ephemeral_pkt->byte_code,(ephemeral_pkt->code_rec+remBytes)*sizeof(char));
                        if(!ephemeral_pkt->byte_code)
                        {
                                LOGPI("Not enough memory\n");
                                return -1;
                        }

                        //memcpy(ephemeral_pkt->byte_code+ephemeral_pkt->size_code, bufTemp, numBytes-readBytes);
                        cp_ptr = ephemeral_pkt->byte_code+ephemeral_pkt->code_rec;
                        //parsedBytes = numBytes - readBytes;
                        while(countBytesCopied < remBytes)
                        {
                                //printf("Copying byte by byte \n");
                                *cp_ptr++ = *ptr++;
                                countBytesCopied++;
                        }


                        ephemeral_pkt->code_rec+=countBytesCopied;
                        readBytes+=countBytesCopied;

//                      LOGPI("Parsed size: %d Bytes - Expected size: %d \n",ephemeral_pkt->code_rec,ephemeral_pkt->size_code);

//                      printf("P--");
//                      dump_ephemeral_status(ephemeral_pkt);
                        if(ephemeral_pkt->code_rec == ephemeral_pkt->size_code)
                        {
                                LOGPI("EOC\n");
                                ephemeral_pkt->eoc = 1;
                                //ephemeral_pkt->ack = 0;
                                //res = 0
                                //break;

                        }
                        res = 0;
                }
                else
                {
                        LOGPI("Unexpected bytes\n");
                        res = -1;
                        break;
                }

                //if(ephemeral_pkt->eoc) break;

        }
        //printf(" Parse - readBytes: %d numBytes: %d\n", readBytes, numBytes);
        return res;
}


int ephemeral_get_class(int sockfd, const char * class_name, void** clBuf)
{

        char bufOut[4*16*1024];
        unsigned char bufTemp[4*16*1024];
        int *bufIn = NULL;
        ssize_t bytesWritten = 0, bytesRead = 0, numBytes = 0;
        ephemeral_stream ephemeral_pkt;
        int res = -1;
        //LOGPI("Hello -- starting a socket client\n");

        if(class_name == NULL)
        {
                LOGPI("Bad Class name\n");
                return -1;

        }

        if(sockfd == -1)
        {
                LOGPI("Bad socket\n");
                return -1;
        }

        strcpy(bufOut,class_name);

        bytesWritten = write(sockfd,bufOut, strlen(bufOut));

         if(bytesWritten < strlen(bufOut))
        {
                LOGPI("write returned bytesWritten:    %zu\n", bytesWritten);
                //return -2;
        }

        LOGPI("Request --> %s\n",bufOut);

        memset(&ephemeral_pkt,0,sizeof(ephemeral_stream));

        while(!ephemeral_pkt.eoc)
        {
                memset(bufTemp, 0,4*16*1024*sizeof(char));

//              printf("B--");
//              dump_ephemeral_status(&ephemeral_pkt);

                numBytes = read(sockfd,bufTemp,sizeof(bufTemp));
                LOGPI("Recieved %zu Byte(s) <--\n",numBytes);

                res = parse(bufTemp,&ephemeral_pkt, numBytes);
                if(res == -1 ||  ephemeral_pkt.cns || ephemeral_pkt.esc)
                {
                        LOGPI("Error Parsing stream\n");
                        return -1;
                }

//              printf("A--");
//              dump_ephemeral_status(&ephemeral_pkt);
                LOGPI("Waiting for class data ... \n");
        }

//        printf("Class data recieve complete\n");
//        printf("magic: %x %x %x %x \n",ephemeral_pkt.byte_code[0],ephemeral_pkt.byte_code[1],ephemeral_pkt.byte_code[2],ephemeral_pkt.byte_code[3]);

        // copy buffer in buffer provided
        //memcpy(clBuf, bufIn,bytesRead);
        *clBuf = ephemeral_pkt.byte_code;
	//LOGPI("Returning Class size: %d, %X",ephemeral_pkt.size_code,ephemeral_pkt.byte_code);
	return ephemeral_pkt.size_code;
}

int ephemeral_get_asset (int sockfd, const char *asset_name, void** clBuf) {
    // char *bufOut;
    char bufOut[4*16*1024];
    unsigned char bufTemp[4*16*1024];
    int *bufIn = NULL;
    ssize_t bytesWritten = 0, bytesRead = 0, numBytes = 0;
    ephemeral_stream ephemeral_pkt;
    int res = -1;

    if (asset_name == NULL) {
        LOGPI("Bad Class name\n");
        return -1;
    }

    if (sockfd == -1) {
        LOGPI("Bad socket\n");
        return -1;
    }

    // bufOut = (char*) malloc(strlen(asset_name));
    strcpy(bufOut, asset_name);

    bytesWritten = write(sockfd, bufOut, strlen(bufOut));
    
    if (bytesWritten < strlen(bufOut)) {
        LOGPI("write returned bytesWritten:    %zu\n", bytesWritten);
        //return -2;
    }

    LOGPI("Request --> %s\n",bufOut);
    memset(&ephemeral_pkt,0,sizeof(ephemeral_stream));

    while (!ephemeral_pkt.eoc) {
        memset(bufTemp, 0,4*16*1024*sizeof(char));
        numBytes = read(sockfd,bufTemp,sizeof(bufTemp));

        LOGPI("Recieved %zu Byte(s) <--\n",numBytes);
        res = parse(bufTemp,&ephemeral_pkt, numBytes);

        if (res == -1 ||  ephemeral_pkt.cns || ephemeral_pkt.esc) {
            LOGPI("Error Parsing stream\n");
            return -1;
        }
        LOGPI("Waiting for asset data ... \n");
    }

    // copy buffer in buffer provided
    //memcpy(clBuf, bufIn,bytesRead);
    *clBuf = ephemeral_pkt.byte_code;
    //LOGPI("Returning Class size: %d, %X",ephemeral_pkt.size_code,ephemeral_pkt.byte_code);
    return ephemeral_pkt.size_code;
}

int ephemeral_get(int sockfd, ephemeral_req_type type,const char * name, void**  buf)
{
	int res = -1;
	switch(type)
	{
		case JCLASS:
			{
				LOGPI("JCLASS\n");
				res = ephemeral_get_class(sockfd,name,buf);
			}
			break;
		case STATIC_ASSET:
			{
				// LOGPI("ASSET\n");
				// res = 0;
                res = ephemeral_get_asset(sockfd, name, buf);
			}
			break;
		default:
			break;
			LOGPI("Some issue - in type of ephemeral request\n");
	}

	if(res > 0) LOGPI("# bytes Returned: %d", res);
	return res;

}

char *repl_str(const char *str, const char *old, const char *NEW) {

    /* Adjust each of the below values to suit your needs. */

    /* Increment positions cache size initially by this number. */
    size_t cache_sz_inc = 16;
    /* Thereafter, each time capacity needs to be increased,
     * multiply the increment by this factor. */
    const size_t cache_sz_inc_factor = 2;
    /* But never increment capacity by more than this number. */
    const size_t cache_sz_inc_max = 1048576;

    char *pret, *ret = NULL;
    const char *pstr2, *pstr = str;
    size_t i, count = 0;
    ptrdiff_t *pos_cache = NULL;
    size_t cache_sz = 0;
    size_t cpylen, orglen, retlen, newlen, oldlen = strlen(old);

    /* Find all matches and cache their positions. */
    while ((pstr2 = strstr(pstr, old)) != NULL) {
        count++;

        /* Increase the cache size when necessary. */
        if (cache_sz < count) {
            cache_sz += cache_sz_inc;
            pos_cache = realloc(pos_cache, sizeof(*pos_cache) * cache_sz);
            if (pos_cache == NULL) {
                goto end_repl_str;
            }
            cache_sz_inc *= cache_sz_inc_factor;
            if (cache_sz_inc > cache_sz_inc_max) {
                cache_sz_inc = cache_sz_inc_max;
            }
        }

        pos_cache[count-1] = pstr2 - str;
        pstr = pstr2 + oldlen;
    }

    orglen = pstr - str + strlen(pstr);

    /* Allocate memory for the post-replacement string. */
    if (count > 0) {
        newlen = strlen(NEW);
        retlen = orglen + (newlen - oldlen) * count;
    } else  retlen = orglen;
    ret = malloc(retlen + 1);
    if (ret == NULL) {
        goto end_repl_str;
    }

    if (count == 0) {
        /* If no matches, then just duplicate the string. */
        strcpy(ret, str);
    } else {
        /* Otherwise, duplicate the string whilst performing
         * the replacements using the position cache. */
        pret = ret;
        memcpy(pret, str, pos_cache[0]);
        pret += pos_cache[0];
        for (i = 0; i < count; i++) {
            memcpy(pret, NEW, newlen);
            pret += newlen;
            pstr = str + pos_cache[i] + oldlen;
            cpylen = (i == count-1 ? orglen : pos_cache[i+1]) - pos_cache[i] - oldlen;
            memcpy(pret, pstr, cpylen);
            pret += cpylen;
        }
        ret[retlen] = '\0';
    }

end_repl_str:
    /* Free the cache and return the post-replacement string,
     * which will be NULL in the event of an error. */
    free(pos_cache);
    return ret;
}
