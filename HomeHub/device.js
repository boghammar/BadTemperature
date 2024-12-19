// -------------------------------------------------------------------------------------
// device.js
// -------------------------------------------------------------------------------------

class Device {
  // -------------------------------------------------------------------------------------
  constructor(id, name, type, ip) {
    this.id = id;
    this.name = name;
    this.type = type;
    this.ip = ip;
    this.getStatus();
  }

  // -------------------------------------------------------------------------------------
  async getStatus() {
    try {
      var url = 'http://' + this.ip + '/rpc/Shelly.Getstatus';
      console.log('device.getStatus: ' + url);
      const response = await fetch(url);
      const data = await response.json();
      console.log(data);
      this.ssid = data.wifi.ssid;
      this.rssi = data.wifi.rssi;

      url = 'http://' + this.ip + '/rpc/switch.Getconfig?id=0';
      console.log('device.getStatus: ' + url);
      const response2 = await fetch(url);
      const data2 = await response2.json();
      console.log(data2);
      this.name = data2.name;

      await this.getPowerState();
      return data;
    } catch (error) {
      console.error('Error:', error);
    }
  }
  // -------------------------------------------------------------------------------------
  async getPowerState() {
    try {
      var url = 'http://' + this.ip + '/rpc/switch.getstatus?id=0';
      console.log('device.getPowerState: ' + url);
      const response = await fetch(url);
      const data = await response.json();
      console.log(data);
      this.output = data.output;
      this.power = data.apower;
    } catch (error) {
      console.error('Error:', error);
    }
  }
  // -------------------------------------------------------------------------------------
  async turnOn(turnon) {
    try {
      var url = 'http://' + this.ip + '/rpc/switch.Set?id=0&on=' + turnon;
      console.log('device.turnon: ' + url);
      const response = await fetch(url);
      const data = await response.json();
      console.log(data);
      await this.getPowerState();
      //this.output = Boolean(turnon); // update internal state
      console.log('device.turnon: ' + this.toString());
      return data;
    } catch (error) {
      console.error('Error:', error);
    }
  }

  // -------------------------------------------------------------------------------------
  toJson() {
    return {
      id: this.id,
      name: this.name,
      type: this.type,
      ip: this.ip,
    };
  }

  // -------------------------------------------------------------------------------------
  toString() {
    return `Device: ${this.id} ${this.name} (${this.type}) ${this.ip} ${this.ssid} ${this.rssi}db ${this.power}W IsON:${this.output}`;
  }
}

module.exports = Device;