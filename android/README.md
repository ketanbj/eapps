# Wandroid: Android modifications to support app streaming

#### libephemeralutils:
1. Added libephemeralutils implementation in system/core and ephemeralutils headers in system/core/include
2. Added cli application cliephemeralutils to test libephemeralutils in external
3. Added custom log support in system/core/include/log/log.h

#### ART - To handle loading of JAVA classes required by ephemeral apps
1. Added ephemeral_linker.cc, ephemeral_linker.h implementing ephemeral class linkers which are loaded from ephemeral app server
2. Modified DexFile_openDexFileNative to request classes for ephemeral apps from ephemeral app server
3. Added ephemeral mode option in ART command line arguements
4. Added ephemeral_linker initialization in runtime.cc
5. Added handler for SIGUSR2 to invoke cleanup
6. Extend ART to support loading of native libraries required by apps
7. Handle SIGUSR2 for ephemeral app clean up

### Asset Manager in androidfw
1. Modified t handle loading static assets required by ephemeral apps

#### rootdir
1. Added ephemeralapp-list to be created to maintain a list of available ephemeral apps
2. Modified init.rc to create ephemeralapps config folder i.e., /data/ephemeralapps and copy ephemeralapp-list as app-list to /data/ephemeralapps/app-list bootup
3. Added ephemeralip-list to be created to maintain a list of IPs of ephemeral app servers
4. Modified init.rc to copy ephemeralip-list to  /data/ephemeralapps/ip-list

#### Ephemeral app cleanup
1. Added a signal SIG_USR2 in core/java/android/os/Process.java

#### SEAndroid policy for ephemeral apps
1. Added new security label, context entry for ephemeral apps in context.te, mac_permissions.te
2. Added new policy in ephemeral app policy ephemeral_app.te

#### Build System: 
1. Add cliephemeralutils in target/product/base.mk and add cliephemeralutils to external
2. Add ephemeralapp-list entry in core/legacy_prebuilts.mk
3. Add system/core/rootdir/ephemeralapp-list:system/ephemeralapp-list in target/product/embedded.mk
4. Add system/core/rootdir/ephemeralip-list:system/ephemeralip-list in target/product/embedded.mk
