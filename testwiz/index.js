// Refs:
// https://github.com/sbidy/pywizlight?tab=readme-ov-file
// 

const os = require('os');

const DEVICE_UDP_LISTEN_PORT = 38900;
const LIGHT_UDP_CONTROL_PORT = 38899;
const IPOFDEVICE = '192.168.20.40'; //90';

const readline = require('readline');
const dgram = require('dgram');

const rl = readline.createInterface({
  input: process.stdin,
  output: process.stdout
});

console.log('Welcome to the cli-app!');
console.log('help for help');
rl.setPrompt('cli-app> ');
rl.prompt();

const listeningSocket = dgram.createSocket('udp4');

let deviceIP = IPOFDEVICE;

listeningSocket.on('message', (msg, rinfo) => {
  console.log(`ListeningSocket Received message: "${msg}" from ${rinfo.address}:${rinfo.port}`);
});

listeningSocket.on('listening', () => {
  const address = listeningSocket.address();
  console.log(`UDP Server listening on ${address.address}:${address.port}`);
});

const sendMessage = (message, address, port = LIGHT_UDP_CONTROL_PORT) => {
    const sendSocket = dgram.createSocket('udp4');
    const msg = Buffer.from(message, 'utf8');
    sendSocket.on('message', (msg, rinfo) => {
      console.log(`Received message: ${msg} from ${rinfo.address}:${rinfo.port}`);
    });
    console.log(sendSocket.eventNames());
    sendSocket.send(msg, 0, msg.length, port, address, (error) => {
      if (error) {
        console.log(`Error sending message: ${error}`);
      } else {
        console.log(`Message sent to ${address}:${port}: ${message}`);
      }
    });
  };

rl.on('line', (line) => {
    // Handle user input here
    const args = line.split(' ');0
    if (line.length > 0) {
        console.log(`You entered: ${line}`);
        switch (args[0]) {
            case 'state':
                sendMessage('{"method": "getPilot"}', deviceIP);
                break;
            case 'power':
                sendMessage('{"method": "getPower"}', deviceIP);
                break;
            case 'info':
                sendMessage('{"method": "getDevInfo"}', deviceIP);
                break;
            case 'wifi':
                sendMessage('{"method": "getWifiConfig"}', deviceIP);
                break;
            case 'on':
                sendMessage('{"method": "setPilot", "params": {"state": true}}', deviceIP);
                break;
            case 'off':
                sendMessage('{"method": "setPilot", "params": {"state": false}}', deviceIP);
                break;
            case 'config':
                sendMessage('{"method": "getUserConfig"}', deviceIP);
                sendMessage('{"method": "getSystemConfig"}', deviceIP);
                sendMessage('{"method": "getModelConfig"}', deviceIP);
                break;
                case 'exit':
                rl.close();
                break;
            case 'ip': {
                deviceIP = args[1];
                break;  
            }
            case 'help': {
                console.log('state - get the state of the current device');
                console.log('power - get the power of the current device');
                console.log('info - get the info of the current device');
                console.log('wifi - get the wifi config of the current device');
                console.log('on - turn the device on');
                console.log('off - turn the device off');
                console.log('config - get the config of the device');
                console.log('exit - exit the cli-app');
                console.log('ip <ip> - set the ip of the device');
                break;
            }
            default:
                console.log('Invalid command');
        }
    }
  rl.prompt();
});

rl.on('close', () => {
  console.log('Goodbye!');
  process.exit(0);
});

listeningSocket.bind(DEVICE_UDP_LISTEN_PORT, '0.0.0.0', () => {
  console.log(`UDP Server listening on port ${DEVICE_UDP_LISTEN_PORT}`);
  listeningSocket.setBroadcast(true); // Enable broadcast reception
  //listeningSocket.setMulticastTTL(128);
  var message = {
    "method": "registration"
    , "id": 1666
    , "params": {
        /*"HomeId": "12"
        ,*/ "PhoneIp": "192.168.20.126"
        , "PhoneMac": "6479f0d3cda9"
        , "Register": true
    }
    };
    console.log(message);
  const msg = Buffer.from(JSON.stringify(message), 'utf8');
  listeningSocket.send(msg, 0, msg.length, LIGHT_UDP_CONTROL_PORT, '255.255.255.255');
});

/*
const os = require('os');
const macaddress = require('node-macaddress');

// Get IP address
const ipAddresses = [];
const networkInterfaces = os.networkInterfaces();
for (const interfaceName in networkInterfaces) {
  const networkInterface = networkInterfaces[interfaceName];
  for (const info of networkInterface) {
    if (info.family === 'IPv4' && !info.internal) {
      ipAddresses.push(info.address);
    }
  }
}
console.log('IP Addresses:', ipAddresses);

// Get MAC address
macaddress.one((err, mac) => {
  if (err) {
    console.error(err);
  } else {
    console.log('MAC Address:', mac);
  }
});
*/