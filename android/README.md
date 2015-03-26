# Android modifications to support app streaming

## TODO changes:
1. Add Stage (ephome) in target/product/sdk.mk

## Android app framework

## androidfw - to handle loading static assets required by ephemeral apps

## Build System: 
1. Add cliephemeralutils in target/product/base.mk and add cliephemeralutils to external
2. Add ephemeralapp-list entry in core/legacy_prebuilts.mk
3. Add system/core/rootdir/ephemeralapp-list:system/ephemeralapp-list in target/product/embedded.mk
4. Add file ephemeralapp-list to 
		
## libephemeralutils, custom logs:
1. Added libephemeralutils implementation in system/core and ephemeralutils headers in system/core/include
2. Added cli application cliephemeralutils to test libephemeralutils in external
3. Added custom log support in system/core/include/log/log.h

## rootdir
1. Added ephemeralapp-list to be created for maintaining a list of available ephemeral apps
2. Modified init.rc to create ephemeralapps config folder i.e., /data/ephemeralapps and copy ephemeralapp-list as app-list to /data/ephemeralapps/app-list bootup
3. Added ephemeralip-list to be created for maintaining a list of IPs of ephemeral app servers
4. Modified init.rc to copy ephemeralip-list to  /data/ephemeralapps/ip-list

## Ephemeral app cleanup
1. Added a signal SIG_USR2 in core/java/android/os/Process.java

## ART - To handle loading of JAVA classes required by ephemeral apps
1. Added ephemeral_linker.cc, ephemeral_linker.h implementing ephemeral class linkers which are loaded from ephemeral app server
2. Modified DexFile_openDexFileNative to request classes for ephemeral apps from ephemeral app server
3. Added ephemeral mode option in ART command line arguements
4. Added ephemeral_linker initialization in runtime.cc
5. Added handler for SIGUSR2 to invoke cleanup

### TODO: ART changes (to be updated):
1. Extend ART to support loading of native libraries required by apps
2. Handle SIGUSR2 for ephemeral app clean up

### Interesting facts about OAT file format: 
1. OAT is not as fast as complete native code. 
2. OAT file is itself a elf object which might contain more than one dex files (it contains them in full) in addiion to compiled code which can be either (i) Fully compiled, (ii) Few methods compiled or (iii) Not compiled at all and JIT still happens if required. So, it seems that an OAT file would be larger than dex or odex files.
3. We could leverage multi dex support OR split the OAT fomat between device specific (dex files) and non-specific parts (compiled portion of classes.dex) to reduce state explosion on eBox in addition to runtime compilation of apps


