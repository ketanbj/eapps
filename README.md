Ephemeral Apps
=================
Ephemeral apps are new types of apps that are (i) immediately and on-demand available
on end-user devices, but (ii) are ephemeral, in that they
can completely disappear from the device once the end-user
leaves the current context. At the same time, (iii) they run
in the same fashion and are able to seamlessly access device
features and provide similar security guarantees as the native
apps installed on end user devices.

The two component which make ephemeral apps possible are:

- 'Wandroid': A Mobile OS built on top of Android with support for app streaming and app ephemerality.

- 'Ephemeral app server': An app streaming server that makes possible decoupling of apps from devices.

This Repository is organized as follows
--------------------------------------------
########
<pre>
<b>eapps</b>
  ├── Changes/Addition to Android Lollipop AOSP v5.0.3 required to support app streaming and app ephemerality in android.
  │   Contains only the changed files which can be used to compile your own wandroid. For compiling android please refer: <a href: https://source.android.com/source/building.html="_blank"</a> Compiling android.
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