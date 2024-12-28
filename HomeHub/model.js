const ShellyDevice = require('./ShellyDevice');
const WizDevice = require('./WizDevice');

class Model {
    constructor() {
        this.devices = [];
        this.name = "MyModel";
    }

    initiate(deviceConfig) {
        deviceConfig.forEach((deviceCfg, index) => {
            let device;

            switch (deviceCfg.type) {
                case "Shelly":
                    device = new ShellyDevice(index, deviceCfg.name, deviceCfg.ip);
                    break;
                case "Wiz":
                    device = new WizDevice(index, deviceCfg.name, deviceCfg.ip);
                    break;
                default:
                    throw new Error(`Unknown device type: ${deviceCfg.type}`);
                    break;
            }
            this.devices[index] = device;
        });

        this.deviceMap = this.devices.reduce((map, device) => {
            map[device.id] = device;
            return map;
        }, {});
    }

    async operateOnDevice(id, operation) {
        switch (operation) {
            case "status":
                return this.deviceMap[id].getStatus()
                .then((data) => { return data; })
                .catch((error) => { return { error: error }; });
            case "on":
                return await this.deviceMap[id].turnOn(true);
            case "off":
                return await this.deviceMap[id].turnOn(false);
            case "power":
                return await this.deviceMap[id].getPowerState();
            default:
                return { error: `Unknown operation: ${operation}`};  
        }   
        return { status: "OK" };
    }
}

module.exports = Model;