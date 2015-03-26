#ifndef __ART_RUNTIME_EPHEMERAL_LINKER_H
#define __ART_RUNTIME_EPHEMERAL_LINKER_H

#include <vector>
#include <string>

#include "dex_file.h"

namespace art{

class EphemeralLinker{
	private:
	std::vector<std::string> apps;
	std::vector<std:: string> ips;
	int sockfd;
	public:	
	explicit EphemeralLinker();
	~EphemeralLinker();
	bool OpenSocket();
	bool ReadEphemeralApps();
	bool FetchDex(const char* sourceName, unsigned char **ppNwBytes,int *pNwLength);
	bool OpenEphemeralDexFiles(const char* dex_location, const char* oat_location,
                           std::vector<std::string>* error_msgs,
                           std::vector<const DexFile*>* dex_files);
	//FIXME: Check if we require mutator lock
};
}
#endif
