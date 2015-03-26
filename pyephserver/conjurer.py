#!/usr/bin/python

from BaseHTTPServer import BaseHTTPRequestHandler,HTTPServer
from os import curdir, sep
import json
from pprint import pprint as pp
import time  
from watchdog.observers import Observer
from watchdog.events import PatternMatchingEventHandler
import Queue as qu
import dummy_thread as Thread
from threading import Timer
import subprocess

import SocketServer
import os
import cgi
import base64
import datetime
import re
import uuid
import urllib

gSettings = {}
gDebug = False
gVerbose = False

gApps = {'length':0,'apps':[]}

def pDebug(msg,usepp=False):
	if gDebug is True:
		if usepp:
			pp('DEBUG:')
			pp(msg)
		else:
			print 'DEBUG: ',msg

def pInfo(msg,usepp=False):
	if gDebug is True and gVerbose is True:
		if usepp:
			pp('INFO: ')
			pp(msg)
		else:
			print 'INFO: ',msg

class onNewAppAdded(PatternMatchingEventHandler):
	patterns=["*.apk"]
	l_events=[]
	u_events=[]
	isTimerStarted = False
		
	def log(self,event):
			self.l_events.append(event)
			pInfo("Logged fs event")
			if self.isTimerStarted == False:
				t = Timer(5, self.cprocess)
				t.start()
				self.isTimerStarted = True

	def cprocess(self):
		# order preserving
		for e in self.l_events:
			if e not in self.u_events:
				self.u_events.append(e)
		del self.l_events[:]

		for e in self.u_events:
			self.process(e)

		del self.u_events[:]
		self.isTimerStarted = False
		#self.t.cancel()
	
	def genconf(self, out):
		appInfo = {}
		if out == '':
			pInfo("No appt output")
			return None
		else:
			lines = out.split('\n')
			for line in lines:
				if 'package' in line and len(line.split(' ')) >=2:
					appInfo['package'] = line.split(' ')[1].split('=')[1].replace('\'','')
				elif 'launchable-activity' in line and len(line.split(' ')) >=2:
					appInfo['activity'] = line.split(' ')[1].split('=')[1].replace('\'','')
				elif 'application:' in line and len(line.split('\'')) >=3:
					appInfo['label'] = line.split('\'')[1]
					appInfo['icon_url'] = "http://130.207.5.67:"+str(gSettings['port'])+'/'+gSettings['xrepo']+appInfo['package']+'/'+line.split('\'')[3].replace('\'','')
				else:
					pass
			return appInfo
		
	def update_app_conf(self, appInfo,new=False):
		global gApps
		#update global app info
		gApps['length']+=1
		gApps['apps'].append(appInfo)
		if new:
			with open(gSettings['appconfig']+appInfo['package']+'.json','w') as conf:
				json.dump(appInfo,conf)
	
	def run_aapt(self, app):
		#TODO: handle aapt exit code properly
		out = ''
		try:
                        pInfo("Aapt command:")
                        cmd = [gSettings['aaptpath']+"aapt", "d", "badging",app]
                        pInfo(cmd)
                        xInfoP = subprocess.Popen(cmd,stdin=subprocess.PIPE,stdout=subprocess.PIPE,stderr=subprocess.PIPE)
                        out,err = xInfoP.communicate()
                except ValueError:
                        pDebug("Issue with aapt")
                        out = ''
			pass

		return out

	def decompress_app(self,appInfo):
		res = True
		#TODO: handle aapt exit code properly
		try:
                        pInfo("7z command:")
                        cmd = [ "7z", "x", gSettings['crepo']+appInfo['package']+".apk","-o"+gSettings['xrepo']+str(appInfo['package']),"-y" ]
                        pInfo(cmd)
                        xInfoP = subprocess.Popen(cmd,stdin=subprocess.PIPE,stdout=subprocess.PIPE,stderr=subprocess.PIPE)
                        out,err = xInfoP.communicate()
			res = True
		except ValueError:
                        pDebug("Issue with aapt")
                        res= False
			pass
		return res

	def process(self, event):
		out=''
        	
        	pInfo(event.src_path)
		pInfo(event.event_type)

		out = self.run_aapt(str(event.src_path))	
		if out != '':
			res = self.genconf(out)
			if res is None:
				pDebug("Unable to generate appInfor"); 
			else:
				pInfo(res,True)
				# write app conf for reboot
				self.update_app_conf(res,True)
				pInfo("App conf updated")
				self.decompress_app(res)
				pInfo("Uncompressed app")
		pInfo(gApps,True)
    
	def on_modified(self, event):
		pInfo("On_modified")
		self.log(event)
	
	def on_created(self, event):
		pInfo("On_created")
		#self.log(event)

	def on_moved(self, event):
		pInfo("On_moved")
		#self.log(event)

	def on_deleted(self, event):
		pInfo("On_deleted")
		#self.log(event)

	def on_init(self):
		apps = os.listdir(gSettings['crepo'])
		uapps = []
		confs = os.listdir(gSettings['appconfig'])
		#FIXME: what happen's if conf file exist & app is updated by copying apk directly
		# Shouldn't we process all apps init time ?
		for conf in confs:
			pInfo(conf)
			appInfo = json.load(open(gSettings['appconfig']+str(conf)))
			# check if conf file exist got this app
			pInfo(gSettings['crepo']+str(conf).replace('.json','.apk'))
			apps.remove(str(conf).replace('.json','.apk'))
			# check if an uncompressed app exists for this app
			if not os.path.isdir(gSettings['xrepo']+str(conf).replace('.json','')):
				uapps.append(str(conf))
			self.update_app_conf(appInfo,False)

		# some apps need preprocessing
		for app in apps:
			out = self.run_aapt(gSettings['crepo']+app)
                	if out != '':
                        	res = self.genconf(out)
                        	if res is None:
                                	pDebug("Unable to generate appInfor");
                        	else:
                                	pInfo(res,True)
                                	# write app conf for reboot
                                	self.update_app_conf(res,True)
                                	pInfo("App conf written")

		# uncompress the apps which are not available 
		for appconf in uapps:
			self.decompress_app(json.load(open(gSettings['appconfig']+appconf)))

		pInfo(gApps,True)
		

#This class will handles any incoming request from the browser 
class onClientRequest(BaseHTTPRequestHandler):	

	def gen_uid(self):
		return uuid.uuid4()

	def parse_params(self, path):
		pDict = {}
		pathd = urllib.unquote(path).decode('utf8')
		epa = pathd.split('?')
		if len(epa) != 2:
			print "malformed URL"
		else:
			eepa = epa[1].split('&')
		for e in eepa:
			kv = e.split('=')
			if len(kv) != 2:
				print("malformed params")
			else:
				pDict[kv[0]] = kv[1]
		pInfo(pDict,True)
		return pDict;
	
	#suppress default logging
	def log_message(self, format, *args):
        	return

	#Handler for the GET requests
	def do_GET(self):
		sendReply = False
		iconext = [".png",".jpg",".jpeg",".ico",".icon"]
		resp = ''
 		
		if self.path.startswith("/eapps"):
			mimetype='application/json'
			sendReply = True
			resp = json.dumps(gApps)
			pInfo(resp)
		
		if self.path.endswith(tuple(iconext)):
			mimetype='image/'+self.path.split('.')[-1]
			sendReply=1
			resp = open(self.path).read()

		if sendReply == True:
			print self.path
			#params = self.parse_params(self.path)
			#print resp

			# send response to client
			if resp is not '':
				self.send_response(200)
				self.send_header('Content-type',mimetype)
				self.send_header('Content-length',len(resp))
				self.end_headers()
				self.wfile.write(resp)
		else:
                    self.send_error(404,'Pyephserver: Unknown request %s' % self.path)
		return

	#Handler for the POST requests
	def do_POST(self):
		print "Post request ... "
		if self.path.startswith("/newapp"):
			#print self.headers
			appname = 'Unknown'
			readPost = True
			params = parse_params(self.path)
			if 'name' not in params.keys():
				pDebug("App name not specified")
				readPost = False
			else:
				appname = params['name']

			if readPost:
				form = cgi.FieldStorage(
					fp=self.rfile, 
					headers=self.headers,
					environ={'REQUEST_METHOD':'POST',
		        		'CONTENT_TYPE':self.headers['Content-Type'],
					},keep_blank_values=1)
				pInfo("Reading post ... ")
				saveFile = True
				# collect app binary from app provider
				try:
					#recid = form['recid'].file.read()
					app = form['app'].file.read()
				except:
					pDebug("Issue in recieving app")
					saveFile = False
					pass


			# send response to client
			self.send_response(200)
                	self.send_header('Content-Type','text/plain')
                	self.send_header('Content-Length',len(appname))
                	self.end_headers()
			self.wfile.write(appname)

			if saveFile: 
				pInfo("Writing app to repository ...")
				recname = gSettings['crepo'] + appname + '.apk'
				fdim = open(appname,'wb')
				#FIXME: handle binary data properly
				fdim.write(base64.b64decode(app));
				fdim.close();
				pInfo("saved app "+ appname)
				pDebug("Post newapp successful")
			else:
				pDebug("Post newapp failed")

		elif self.path.startswith("/update"):
			#TODO: handle app update here
			pass
		else:
			pDebug("Unknown post request -- :-/")
		return			
			
def check_settings(s):
	global gDebug
	gDebug = s['debug']

	global gVerbose
	gVerbose = s['verbose']

	if not os.path.isdir(s['crepo']):
		os.makedirs(s['crepo'])
		pInfo("Created crepo - No apps found on server")

	if not os.path.isdir(s['xrepo']):
		os.makedirs(s['xrepo'])
		pInfo("Created xrepo - No preprocessed apps")

	if not os.path.isdir(s['appconfig']):
		os.makedirs(s['appconfig'])
		pInfo("Created app repository - No available apps")
	
	if not os.path.isdir(s['aaptpath']):
		pInfo("No aapt found to preprocess apps")

	if not isinstance(s['port'],(int,long)):
		pDebug("Port not properly specified")
		return False
	print gApps
	return True;

class ThreadedHTTPServer(SocketServer.ThreadingMixIn, HTTPServer):
    """Handle requests in a separate thread."""

if __name__ == '__main__':
	global gSettings
	gSettings = json.load(open("conf.json"))

	if check_settings(gSettings):
		pInfo(gSettings,True)
	else:
		pDebug("Error in configuration");

	observer = Observer()
	ohandler = onNewAppAdded()
	ohandler.on_init()
	observer.schedule(ohandler,path=gSettings['crepo'])
	observer.start()

	try:
		server = ThreadedHTTPServer(('130.207.5.67', gSettings['port']), onClientRequest)
        	print 'Starting server on port', gSettings['port'], 'use <Ctrl-C> to stop'
        	server.serve_forever()
	except KeyboardInterrupt:
		print 'Stopping server...'
		observer.stop()
		print 'Stopping watch on app repo...'

	observer.join()
