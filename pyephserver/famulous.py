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
from stat import *
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

def check_settings(s):
	global gDebug
	gDebug = s['debug']

	global gVerbose
	gVerbose = s['verbose']

	if not isinstance(s['port'],(int,long)):
		pDebug("Port not properly specified")
		return False
	return True;


class MyTCPHandler(SocketServer.StreamRequestHandler):
	"""
    	The RequestHandler class for our server.

    	It is instantiated once per connection to the server, and must
    	override the handle() method to implement communication to the
    	client.
    	"""

	def send_code(self, code):
        	self.wfile.write(code)

	def send_valid(self,path):
		buf = open(path,'r').read()
		self.wfile.write(len(buf))	
        	self.wfile.write(buf)

	def check_send(self, path):
		pInfo("Looking up:")
		pInfo(path)
		full_path = os.path.join(gSettings['xrepo'],path)	
		mode = os.stat(full_path).st_mode
		if stat.S_ISREG(mode):
			self.send_valid(full_path)
		else:
			pDebug("Sending CNS - Could not stat requested file")
			self.send_code(0xE3)

     	def handle(self):
        	# self.rfile is a file-like object created by the handler;
        	# we can now use e.g. readline() instead of raw recv() calls
        	self.data = self.rfile.readline().strip()
        	print "Request: ".format(self.client_address[0])
        	print self.data

		t_req = self.data.split('}')
		for req in t_req:
			self.send_code(0xFF);
			pInfo("Sent ACK")
			pInfo(req)
			if not "classes" in req:
				pInfo("Asset request")
			else:	
				pInfo("Class request")
			preq = req[:11] + '/classes.dex'
			pInfo(preq)
			self.check_send(preq)

if __name__ == "__main__":
    
    	global gSettings
	gSettings = json.load(open("conf.json"))

	if check_settings(gSettings):
		pInfo(gSettings,True)
	else:
		pDebug("Error in configuration");
    	# Create the server, binding to localhost on port 9999
    	server = SocketServer.TCPServer(('130.207.5.67', gSettings['port']), MyTCPHandler)

    	# Activate the server; this will keep running until you
    	# interrupt the program with Ctrl-C
    	server.serve_forever()

	
