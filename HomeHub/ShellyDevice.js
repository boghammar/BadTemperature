const Device = require('./device');
/** A Shelly Wifi device
 * @param {string} id - The id (internal) of the device
 * @param {string} name - The name of the device. Will be displayed in the UI and overridden the name of the device if set
 * @param {string} ip - The ip of the device
 */
class ShellyDevice extends Device {
    constructor(id, name, ip) {
        super(id, name, "Shelly", ip);
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
      return this;
    } catch (error) {
      console.error('Error:', error);
    }
  }
  // -------------------------------------------------------------------------------------
  async turnOn(turnon) {
    try {
      var url = 'http://' + this.ip + '/rpc/switch.Set?id=0&on=' + turnon;
      console.log('shelly.turnon: ' + url);
      const response = await fetch(url);
      const data = await response.json();
      console.log(data);
      await this.getPowerState();
      console.log('shelly.turnon: ' + this.toString());
      let ret = {"success": true, "message": data};
      if (data.code !== undefined) {
        ret.success = false;
      }
      return ret;
    } catch (error) {
      console.error('Shelly.turnon Error:', error);
      return {"success": false, "error": error};
    }
  }

}

module.exports = ShellyDevice;