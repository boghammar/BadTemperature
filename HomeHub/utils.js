const network = require('network');
const os = require('os');
const networkInterfaces = os.networkInterfaces();

class Utils {

  getSubnet() {
    Object.keys(networkInterfaces).forEach(interfaceName => {
      const iface = networkInterfaces[interfaceName];
      iface.forEach(address => {
        if (address.family === 'IPv4' && !address.internal) {
          return address.address;
        }
      });
    })
  }

  async getActiveSubnet() {
    return new Promise( (resolve, reject) => {
      network.get_active_interface( (err, iface) => {
        if (err) {
          reject(err);
        } else {
          const subnet = `${iface.ip_address}/${iface.netmask}`;
          console.log(`Subnet: ${subnet}`);
          resolve(subnet);
        }
      });
    });
  }

  async  getActiveSubnetSync() {
    while (!subnet) {
      try {
        return await this.getActiveSubnet();
      } catch (error) {
        //throw error;
      }
      await new Promise( resolve => setTimeout(resolve, 10) );
    }
    return subnet;
  }
}

module.exports = Utils;
