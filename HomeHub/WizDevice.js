const DEVICE_UDP_LISTEN_PORT = 38900;
const LIGHT_UDP_CONTROL_PORT = 38899;

const Device = require('./device');
const dgram = require('dgram');

/** A Wiz Wifi device
 * @param {string} id - The id (internal) of the device
 * @param {string} name - The name of the device. Will be displayed in the UI and overridden the name of the device if set
 * @param {string} ip - The ip of the device
 */
class WizDevice extends Device {
    constructor(id, name, ip) {
        super(id, name, "Wiz", ip);
    }
  // -------------------------------------------------------------------------------------
  async getStatus() {
    const sendSocket = dgram.createSocket('udp4');
    const message = {"method": "getPilot"};
    const msg = Buffer.from(JSON.stringify(message), 'utf8'); 
    sendSocket.on('message', (msg, rinfo) => {
        console.log(`Received message: ${msg} from ${rinfo.address}:${rinfo.port}`);
        let json = JSON.parse(msg.toString());
        this.rssi = json.result.rssi;
        this.output = json.result.state;
      });

    sendSocket.send(msg, 0, msg.length, LIGHT_UDP_CONTROL_PORT, this.ip, (error) => {
        if (error) {
            console.log(`Error sending message to ${this.ip}: ${error}`);
        } else {
            console.log(`Message sent to ${this.ip}:${LIGHT_UDP_CONTROL_PORT}: ${message}`);
        }
    });
  }
  // -------------------------------------------------------------------------------------
  async getPowerState() {
    throw new Error('Method not implemented');
  }
  // -------------------------------------------------------------------------------------
  async turnOn(turnOn) {
    const sendSocket = dgram.createSocket('udp4');
    const message = {"method": "setPilot", "params": {"state": turnOn === 'true' ? true : false}};
    const msg = Buffer.from(JSON.stringify(message), 'utf8'); 

    return new Promise((resolve, reject) => {
      sendSocket.on('message', (msg, rinfo) => {
        console.log(`Received message: ${msg} from ${rinfo.address}:${rinfo.port}`);
        let json = JSON.parse(msg.toString());
        resolve(json.result);
      });

      sendSocket.on('error', (err) => {
        console.log(`Socket error ${this.ip}: ${err}`);
        reject({"success": false, "message": err});
      });

      sendSocket.send(msg, 0, msg.length, LIGHT_UDP_CONTROL_PORT, this.ip, (error) => {
        if (error) {
            console.log(`Error sending message to ${this.ip}: ${error}`);
            reject({"success": false, "message": error});
        } else {
            console.log(`Message sent to ${this.ip}:${LIGHT_UDP_CONTROL_PORT}: ${JSON.stringify(message)}`);
        }
      });
    })
  }
}

module.exports = WizDevice;
