#!/usr/bin/python

import requests
import re
import urllib2
import datetime

url = "http://130.207.5.67:3000/eapps"

request = urllib2.Request(url)
response = urllib2.urlopen(request)
page = response.read()

print page
