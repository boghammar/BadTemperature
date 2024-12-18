// device.js

class Device {
    constructor(id, name, type, ip) {
      this.id = id;
      this.name = name;
      this.type = type;
      this.ip = ip;
      this.getStatus();
    }

    async getStatus() {
        try {
            const response = await fetch('http://'+this.ip+'/rpc/Shelly.Getstatus');
            const data = await response.json();
            console.log("Indevice: ");
            console.log(data);
            this.output = data['switch:0'].output;
            this.power = data['switch:0'].apower;
            this.ssid = data.wifi.ssid;
            this.rssi = data.wifi.rssi;
            const response2 = await fetch('http://'+this.ip+'/rpc/switch.Getconfig?id=0');
            const data2 = await response2.json();
            console.log(data2);
            this.name = data2.name;
            return data;
          } catch (error) {
            console.error('Error:', error);
          }
    }
    toJson() {
      return {
        id: this.id,
        name: this.name,
        type: this.type,
        ip: this.ip,
      };
    }

    toString() {
      return `Device: ${this.id} ${this.name} (${this.type}) ${this.ip}`;
    }
  }
  
  module.exports = Device;