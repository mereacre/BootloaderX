var com = require("serialport");
var net = require('net');
var SerialPort = com.SerialPort;

var client = new net.Socket();
var serialPort = new SerialPort("/dev/tty.usbserial-FTWOHPEA", {
	baudrate: 9600,
	parser: com.parsers.raw
});

serialPort.on("open", function () {
	console.log('Port Open');
	serialPort.on('data', function(data) {
		console.log('Data received from port: ' + data);
		console.log('Data received from port (hex): ' + data.toString('hex'));
		client.write(data);
	});

	serialPort.on("close", function(data) {
		console.log('Port Closed');
	});

	client.connect(13377, '127.0.0.1', function() {
		console.log('Connected to server');
		//client.write('Hello, server! Love, Client.');
	});

	client.on('data', function(data) {
		console.log('Received from server: ' + data);
		console.log('Received from server (hex): ' + data.toString('hex'));
		serialPort.write(data, function(err, results) {
			console.log('Write to port error: ' + err);
			console.log('Write to port results: ' + results);
		});
		//client.write(data);
		//client.destroy(); // kill client after server's response
	});

	client.on('close', function() {
		console.log('Client connection closed');
	});
});