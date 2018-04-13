
var http = require('http').createServer(handler); //require http server, and create server with function handler()
var fs = require('fs'); //require filesystem module
var io = require('socket.io')(http);

function handler (req, res) { //create server
	fs.readFile('/var/www/html/index.html', function(err, data) { //read file index.html in public folder
		if (err) {
			res.writeHead(404, {'Content-Type': 'text/html'}); //display 404 on error
			return res.end("404 Not Found");
		} 
		else{
			res.writeHead(200, {'Content-Type': 'text/html'}); //write HTML
			//res.write("The date and time are "+Date());
			res.write(data,"utf8"); //write data from index.html
			res.end();
		}
	});
}

//var execSync = require('child_process').execSync;
//var result = execSync("/var/www/html/main.py").tostring.split('\n');

var child = require('child_process').spawn('python', ['getdata.py'], {
	stdio:[null,null,null,'ipc']
});

http.listen(8080); //listen to port 8080
io.listen(http);
var t = 0;
var h = 0;
var l = 0;
var s = 0;
var status = "normal";
//var status = 0;

child.on('message', function(message) {
	console.log('Receive message...');
	//t = JSON.stringify(message[0]);
	//h = JSON.stringify(message[1]);
	//l = JSON.stringify(message[2]);
	//s = JSON.stringify(message[3]);
	t = 20;
	//status = JSON.stringify(message[0]);
	console.log(message);
	
	io.sockets.on('connection', function(socket){
		setInterval(function() {
			socket.emit('date', {'date': new Date().toLocaleTimeString()});
		}, 1000);
		
		setInterval(function() {			
			//socket.emit('data1', {'data1': t});
			//socket.emit('data2', {'data2': h});
			//socket.emit('data3', {'data3': l});
			//socket.emit('data4', {'data4': s});
			socket.emit('data1', {'data1': t});
			socket.emit('data2', {'data2': 2});
			socket.emit('data3', {'data3': 3});
			socket.emit('data4', {'data4': 4});
			socket.emit('data5',{'data5': status});
		}, 2000);
		
		console.log("bicom test");
		var value;
		var value2;
		socket.on('client_data', function(data) { //get standard from client
			value = data.letter; //transmit this parameter as standarad to change the font color
			console.log(data.letter);
			if(t > value) {
				//console.log(value);
				console.log(t > value);	
				socket.emit('flag1', {'flag1': "red"});
				//socket.emit('temp_info', {'temp_info': "The temperature is too high!"})
			} else {
				socket.emit('flag1', {'flag1': "black"});
				//socket.emit('temp_info', {'temp_info': "The temperature is normal."})
			}		 
		});
		socket.on('client_data2', function(data) { //get standard from client
			value2 = data.letter2; //transmit this parameter as standarad to change the font color
			console.log(data.letter2);	
			if (t < value2) {
				socket.emit('flag1', {'flag1': "blue"}); 
				//socket.emit('temp_info', {'temp_info': "The temperature is too low!"})
			} else {
				//socket.emit('flag1', {'flag1': "black"});
				}
		});
	});
	
}); 
