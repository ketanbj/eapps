Ephemeral Apps
=================
Ephemeral apps are new types of apps that are (i) immediately and on-demand available
on end-user devices, but (ii) are ephemeral, in that they
can completely disappear from the device once the end-user
leaves the current context. At the same time, (iii) they run
in the same fashion and are able to seamlessly access device
features and provide similar security guarantees as the native
apps installed on end user devices.

The two component that make ephemeral apps possible are:
- **Wandroid**: Enhanced Android with support for app streaming and app ephemerality.
- **Ephemeral app server**: A two phased app streaming server to decouple of apps from devices.

Demo
------
<a href="http://www.youtube.com/watch?feature=player_embedded&v=ggEDYt9Wdsw
" target="_blank"><img src="http://img.youtube.com/vi/ggEDYt9Wdsw/0.jpg" 
alt="IMAGE ALT TEXT HERE" width="240" height="180" border="10" /></a>

Repository organization
--------------------------------------------
########
<pre>
<b>eapps</b>
  ├── <b>android</b>
  │		├─ Changes/Addition to Android Lollipop AOSP v5.0.3 required to support 
  │     │  app streaming and app ephemerality in android.
  │     │  Contains only the changed files which can be used to compile your own 
  │     │  Wandroid. For compiling android please refer: <a href: https://source.android.com/source/building.html="_blank"</a> Compiling android.
  │ 
  ├── <b>pyephserver</b>
  │     ├─ Ephemeral app server implemented using python
  ├─ <b>ephserver</b> 
  │		├─Ephemeral app server implemented using node.js
  │
..
</pre> 


License
---------
Refer LICENSE.md