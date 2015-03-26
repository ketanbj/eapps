/*Simple socket based server
	Author: Ketan Bhardwaj
*/
var net = require('net');
var fs = require('fs');
var buffer = require('buffer');

var settings = {};
try {
  settings = require("konphyg")(__dirname)("config");
} catch (e) {
  console.error(e);
}

var debug = settings.debug;
var verbose = settings.verbose;

var send_code = function(sock,code)
{
	buf = new Buffer(1);
	buf[0] =code;
	sock.write(buf);
}

var send_size = function(sock,size)
{
	//var tosend = parseInt(size);
        var buf = new Buffer(4);
   	//bytes[0] = tosend & 24;
   	//bytes[1] = tosend >> 16;
   	//bytes[2] = tosend >> 8;
   	//bytes[3] = tosend;
	//buf = new Buffer(bytes);
        buf.writeInt32LE(size, 0);
	if(verbose) console.log("send_size size: "+ size );
        sock.write(buf);
}


var lookup_class = function(sock,class_name)
{
	//TODO Check
	var class_path = settings.xrepo + class_name.toString().replace('.apk.classes','/classes.dex');
	if(verbose) console.log("lookup_class() class path: "+ class_path);

	try {
		stat = fs.statSync(class_path);

		if (!stat.isFile()) {
			if(verbose) console.log("lookup_class(): unable to stat file " + class_path);
			return false;
		}
		else
		{
			send_size(sock,stat.size);
		}
	} catch (e) {
		if(debug) console.log("lookup_class(): FS STAT ERROR");
		return false;
	}
	if(verbose) console.log("lookup_class() : Found ");
	return true;
}

var lookup_asset = function(sock,asset_name)
{
	//TODO Check
	var asset_path = settings.xrepo + asset_name.toString().replace('.apk','');
	if(verbose) console.log("lookup_asset() asset path: "+ asset_path);

	try {
		stat = fs.statSync(asset_path);

		if (!stat.isFile()) {
			if(verbose) console.log("lookup_asset(): unable to stat file " + asset_path);
			return false;
		}
		else
		{
			send_size(sock,stat.size);
		}
	} catch (e) {
		if(debug) console.log("lookup_asset(): FS STAT ERROR");
		return false;
	}
	if(verbose) console.log("lookup_asset() : Found ");
	return true;
}

var send_class_byte_code = function(sock,class_name)

{
	//TODO Check
	var class_path = settings.xrepo + class_name.toString().replace('.apk.classes','/classes.dex');
        if(verbose) console.log("send_class_byte_code() Class path: "+class_path);
	
	readable = fs.createReadStream(class_path, { flags: "r"});
	readable.pipe(sock,{end:false});

	readable.on('end',function(){
//		readable.unpipe(sock);
		if(verbose) console.log("send_class_byte_code(): Sending <EOC>");
//		send_code(sock,0xFC);
	});

}

var send_asset = function(sock,asset_name)

{
	//TODO Check
	var asset_path = settings.xrepo + asset_name.toString().replace('.apk','');
        if(verbose) console.log("send_asset() asset path: "+asset_path);
	
	readable = fs.createReadStream(asset_path, { flags: "r"});
	readable.pipe(sock,{end:false});

	readable.on('end',function(){
//		readable.unpipe(sock);
		if(verbose) console.log("send_asset_byte_code(): Sending <EOC>");
//		send_code(sock,0xFC);
	});

}


var server = net.createServer(function(c) { 

	//'connection' listener
	if(verbose) console.log("c.on(connected)");

	// class name is strings
	c.setEncoding('hex');

	c.on('end', function() {
		if(verbose) console.log('c.on(end): disconnected');
  	});

	c.on('data', function(data){
		raw = new Buffer(data,'hex');
      		if(verbose) console.log("c.on(data): request for: " + raw.toString());
      		
		send_code(c,0xFF);
		if(raw.toString().indexOf(".classes") > -1)
		{
			if(lookup_class(c,raw.toString().substring(0, raw.toString().length - 1)))
 			{
			// send class from here asynchronously
			send_class_byte_code(c,raw.toString().substring(0, raw.toString().length - 1));/*,function(err){
			if(err)
				{
					console.log(" Error sending class");
					c.write(0xE4);
				}
			});*/

			}
			else
			{	
    				if(debug) console.log(" c.on(data): unable to locate class -- Sending <CNS>") ;
				send_code(c,0xE3);
			}
		}
		else
		{
			if(lookup_asset(c,raw.toString().substring(0, raw.toString().length - 1)))
			{
				send_asset(c,raw.toString().substring(0, raw.toString().length - 1));
			}
			else
			{
				if(debug) console.log(" c.on(data): unable to locate asset -- Sending <CNFC>") ;
				send_code(c,0xE3);
			}
		}
		
	
	});

	c.on('error', function(e) {
		console.log("socket " + e);
	});

});

server.on('error', function (e) {
	if (e.code == 'EADDRINUSE') {
		if(debug) console.log("TCP server.on(error): Address in use, retrying...");
 		setTimeout(function () {
		server.close();
		server.listen(PORT, HOST);
		}, 1000);
	}
});

server.listen(3000,"192.168.1.204",function() { //'listening' listener
  address = server.address();
  console.log(" Famulous listening on %j", address);
});
