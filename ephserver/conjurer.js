/*Simple HTTP based ephemeral app server
	Author: Ketan Bhardwaj
*/
var http = require('http');
var url = require('url');
var net = require('net');
var JsonSocket = require('json-socket');
var fs = require('fs');
var chokidar = require('chokidar');

var exec = require('child_process').exec,
    child;

var settings = {};
try {
  settings = require("konphyg")(__dirname)("config");
} catch (e) {
  console.error(e);
}
console.log(settings);
var gDebug = settings.debug;
var gVerbose = settings.verbose;
var gCrepo = settings.crepo;
var gXrepo = settings.xrepo;
var gPort = settings.port;
var gAppconfig = settings.appconfig;

var gApps = {'length':0, 'apps':[]}

var gIsInit = true;
var gAppWatcher = chokidar.watch(gCrepo, {ignored: /^\./, persistent: true});

var process_new_app = function(path, stdout)
{
	child = exec('./aapt d badging '+path,
                function (error, stdout, stderr) {
                //console.log('stdout: ' + stdout);
                //console.log('stderr: ' + stderr);
                if (error !== null) {
                        console.log('exec error extracting app info: ' + error);
			return;
                        }

                appInfo = {};
                out = stdout.split('\n');
                console.log(out.length)
                for(i = 0; i < out.length; i++)
                {

                        if(out[i].length > 1)
                        {
                                if(out[i].indexOf('package') > -1 && out[i].split(' ').length >= 2)
                                {
                                        appInfo['package'] = out[i].split(' ')[1].split('=')[1].     replace(/\'/g,'');
                                }
                                else if(out[i].indexOf('launchable-activity') > -1 && out[i].split(' ').length >= 2)
                                {
                                        appInfo['activity'] = out[i].split(' ')[1].split('=')[1].    replace(/\'/g,'');
                                }
                                else if(out[i].indexOf('application:') > -1 && out[i].split('\'').   length >= 3)
                                {
                                        appInfo['label'] = out[i].split('\'')[1];
                                        appInfo['icon_url'] = "http://130.207.5.67:"+gPort+'/'+      appInfo['package']+'/'+out[i].split('\'')[3].replace(/\'/g,'');
                                }
                                else
                                {
                                        ;//console.log("Doing nothing");
                                }

                        }

                }
		if(!('activity' in appInfo))
		{
			appInfo['activity'] = "defaultactivity";
		}
		gApps.apps[gApps.length] = appInfo;
		gApps.length++;
		 //TODO: Extract the app to keep it in uncompressed format
                child = exec('7z x '+path+ " -o"+gXrepo+appInfo['package'],
                        function (error, stdout, stderr) {
                        if (error !== null) {
                                console.log('error uncompressing app: ' + error);
                                }
                });
 		console.log(appInfo);
                fs.writeFile(gAppconfig+appInfo['package']+'.json',JSON.stringify(appInfo),function  (err) {
                         if (err) throw err;
                                console.log('Appconfig saved!');});

                });
}



gAppWatcher.on('add', function(path) {
	console.log('File', path, 'has been added');
	//Only valid at startup time 
	if(gIsInit == true)
	{
		apps  = fs.readdirSync(gCrepo)
		console.log(apps);
		for(i =0; i < apps.length; i++)
		{
			if(apps[i].indexOf(String('.apk'), apps[i].length - String(".apk").length) !== -1)
			process_new_app(gCrepo + apps[i]);
		}
        	gIsInit = false;
	}
	})

var to = null;

gAppWatcher.on('change', function(path) {
	console.log('File', path, 'has been changed');
	//TODO: Handle new incoming apps here 
	// Wait for app to finish downloading before processing it 
	// otherwise aapt & 7z will spit out errors
	if(to != null)
	{
		clearTimeout(to);
		console.log("Waiting for download");
	}

	to = setTimeout(function(){
		console.log("Hope that download is complete");
		process_new_app(path);},100);
	})

gAppWatcher.on('unlink', function(path) {
	console.log('File', path, 'has been removed');
	//TODO: Remove config file from config dir
	})

gAppWatcher.on('error', function(error) {
	console.error('Error happened while watching repo', error);
	
	})


var send_eapp_list = function(req,res)
{

  res.writeHead(200, {'Content-Type': 'application/json','Content-Length': JSON.stringify(gApps).length});
  res.write(JSON.stringify(gApps));
  res.end('');
}

var send_icon = function(req,res,iconname)
{
     var img = fs.readFileSync(gXrepo+iconname);
     res.writeHead(200, {'Content-Type': 'image/png','Content-length': img.length });
     res.end(img, 'binary');
}

var handle_req = function(req,res)
{

	var path = url.parse(req.url).pathname;
	console.log(path);

	if(path == '/eapps' || path =='/')
	{
		send_eapp_list(req,res);
	}
	else if(path == '/favicon.ico')
	{
		console.log("Do nothing for favicon.ico");
	}
	else
	{
		send_icon(req,res,path);
	}
	
}

http.createServer(handle_req).listen(gPort, '130.207.5.67');
console.log('Server running');

