// Min max temperatures
const thermometerMax = 35;
const thermometerWarm = 15;
const thermometerMin = -20;

// Colors for gradient
const minColor = [173, 216, 230]; // lightblue[0, 0, 255]; //  blue 
const zeroColor = [255, 255, 255]; // 0 deegres
const warmColor = [255, 255, 0]; // around 15 deegres
const maxColor = [255, 0, 0]; // red

// -------------------------------------------------------------------------------------
function getInfo(query, elementId, formatFnc) {
    fetch(query)
        .then(function (response) {
            return response.text();
        })
        .then(function (data) {
            if (undefined === formatFnc) {
                return data;
            }
            return formatFnc(data);
        })
        .then(function (data) {
            document.getElementById(elementId).innerHTML = data;
        })
        .catch(function (error) {
            console.error('Error:', error);
        });
}

// -------------------------------------------------------------------------------------
// Color functions
// -------------------------------------------------------------------------------------
function interpolateColor(color1, color2, factor) {
  if (arguments.length < 3) {
    factor = 0.5;
  }
  const result = color1.slice();
  for (let i = 0; i < 3; i++) {
    result[i] = Math.round(result[i] + factor * (color2[i] - color1[i]));
  }
  return result;
}

// -------------------------------------------------------------------------------------
function getTemperatureColor(temp, minTemp, maxTemp) {
  const startColor = minColor; // very cold
  const endColor = maxColor;   // very hot
  let color;
  if (temp <= 0) {
    const factor = (temp - minTemp) / (0 - minTemp);
    color = interpolateColor(minColor, zeroColor, factor);
  } else if (temp > 0 && temp <= thermometerWarm) {
    const factor = (temp - 0) / (thermometerWarm - 0);
    color = interpolateColor(zeroColor, warmColor, factor);
  } else if (temp > 15) {
    const factor = (temp - thermometerWarm) / (maxTemp - thermometerWarm);
    color = interpolateColor(warmColor, maxColor, factor);
  } 
  //const factor = (temp - minTemp) / (maxTemp - minTemp);
  //return interpolateColor(startColor, endColor, factor);
  return color;
}

// -------------------------------------------------------------------------------------
function temperatureFill(min, max, value) {
  return `${((value - min) / (max - min)) * 100}%`;
}

// -------------------------------------------------------------------------------------
function temperatureGradient(value, lastColor) {
  let str = `linear-gradient(to top, rgb(${minColor[0]}, ${minColor[1]}, ${minColor[2]})`;
  if (value > 0) str += `, rgb(${zeroColor[0]}, ${zeroColor[1]}, ${zeroColor[2]})`;
  if (value > thermometerWarm) str += `, rgb(${warmColor[0]}, ${warmColor[1]}, ${warmColor[2]})`;
  str += `, rgb(${lastColor[0]}, ${lastColor[1]}, ${lastColor[2]})`;
  str += `)`;
  return str;   
}
// -------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------
// Sample data, replace with actual data fetching logic
const airTemperature = 35; // Replace with actual air temperature data
const waterTemperature = 19; // Replace with actual water temperature data

const airColor = getTemperatureColor(airTemperature, thermometerMin, thermometerMax);
const waterColor = getTemperatureColor(waterTemperature, thermometerMin, thermometerMax);
console.log(`Air Temperature: ${airTemperature}°C, RGB: rgb(${airColor[0]}, ${airColor[1]}, ${airColor[2]})`);
console.log(`Water Temperature: ${waterTemperature}°C, RGB: rgb(${waterColor[0]}, ${waterColor[1]}, ${waterColor[2]})`);

document.getElementById('air-temp').innerText = `${airTemperature}`;
document.getElementById('water-temp').innerText = `${waterTemperature}`;
document.getElementById('air-temp').style.color = `rgb(${airColor[0]}, ${airColor[1]}, ${airColor[2]})`;
document.getElementById('water-temp').style.color = `rgb(${waterColor[0]}, ${waterColor[1]}, ${waterColor[2]})`;

// Update thermometer fill height based on temperature
document.getElementById('air-thermometer-fill').style.height = temperatureFill(thermometerMin, thermometerMax, airTemperature); //`${(airTemperature / maxTemperature) * 100}%`;
document.getElementById('water-thermometer-fill').style.height = temperatureFill(thermometerMin, thermometerMax, waterTemperature); //`${(waterTemperature / maxTemperature) * 100}%`;

document.getElementById('air-thermometer-fill').style.background = temperatureGradient(airTemperature, airColor);
document.getElementById('water-thermometer-fill').style.background =  temperatureGradient(waterTemperature, waterColor);

getInfo('/info?what=hostname', 'hostname');
getInfo('/info?what=version', 'version');
