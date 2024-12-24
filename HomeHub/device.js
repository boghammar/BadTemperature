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

  async getStatus() {
    throw new Error('Method not implemented');
  }

  async getPowerState() {
    throw new Error('Method not implemented');
  }

  async turnOn(turnOn) {
    throw new Error('Method not implemented');
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