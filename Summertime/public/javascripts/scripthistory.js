import { getTemperatureData } from './script.js';

var myChart;

const ctx = document.getElementById('myChart');

const site = 'norrtorp';
const airlocation = 'shore';
const waterlocation = 'lake';

var airTemp;
var waterTemp;

function getLastValues(range) {
    getTemperatureData(site+'/'+airlocation, (myData) => {
        airTemp = myData;
        getTemperatureData(site+'/'+waterlocation, (myData) => {
            waterTemp = myData;
            const data = myChart.data;
            data.datasets[0].data = airTemp;
            data.datasets[1].data = waterTemp;
            myChart.options.plugins.title.text = `Temperature history last ${range}`;
            myChart.update();
        }, range);
    }, range);
}

var scales = {
        x: {
            type: 'time',
            time: {
                tooltopFormat: 'YYYY-MM-DD HH:mm:ss'
            },
            title: {
                display: true,
                text: 'Time'
            },
            ticks: {
                callback:  function(value, index, values) {
                    var max = moment(this.max);
                    var min = moment(this.min);
                    var diff = max.diff(min, 'hours');
                    var fmt = 'H:mm';
                    if (diff > 48) {    
                        fmt = 'DD/MM H:mm';
                    } else if (diff > 24*7) {
                        fmt = 'DD/MM';
                    } else if (diff > 24*30-1) {
                        fmt = 'YY-MM-DD';
                    }
                    return moment(value).format(fmt);
                }
            }
        },
        y: {
            max: 35,
            title: {
                display: true,
                text: 'Temperature °C'
            }
        }
};
var options = {
    maintainAspectRatio: false,
    scales: scales,
    plugins: {
        title: {
            display: true,
            text: 'Temperature history last week'
        },
        subtitle: { 
            display: true,
            text: site
        },
        legend: {
            display: true,
            position: 'chartArea'
        }
    },
    parsing: {
        xAxisKey: 'timeHuman',
        yAxisKey: 'value'
    }
};

function createChart(myData) {
    console.log(myData);
    airTemp = myData;
    getTemperatureData(site+'/'+waterlocation, (wdata) => {
        waterTemp = wdata;
        myChart = new Chart(ctx, {
            type: 'line',
            data: {
                datasets: [
                    {
                        label: airlocation,
                        borderColor: 'rgb(5, 129, 46)',
                        data: airTemp,
                        cubicInterpolationMode: 'monotone',
                        tension: 0.4
                    },
                    {
                        label: waterlocation,
                        borderColor: 'rgb(23, 7, 243)',
                        data: waterTemp,
                        cubicInterpolationMode: 'monotone',
                        tension: 0.4
                    }
                ]
            },
            options: options
        });

        document.getElementById('lastHour').addEventListener('click', () => getLastValues('hour'));
        document.getElementById('lastDay').addEventListener('click', () => getLastValues('day'));
        document.getElementById('lastWeek').addEventListener('click', () => getLastValues('week'));
        document.getElementById('lastMonth').addEventListener('click', () => getLastValues('month'));

    }, 'week');
}

// ---------------------------------------- Execute this on load
// scripthistory.js

getTemperatureData(site+'/'+airlocation, createChart, 'week');
