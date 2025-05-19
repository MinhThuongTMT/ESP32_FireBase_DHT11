// Chart configuration and setup
export class SensorChart {
    constructor(canvasId) {
        this.ctx = document.getElementById(canvasId).getContext('2d');
        this.maxDataPoints = 20;
        this.updateInterval = 1000;
        this.chartData = {
            labels: Array(this.maxDataPoints).fill(''),
            temperatures: Array(this.maxDataPoints).fill(null),
            humidities: Array(this.maxDataPoints).fill(null)
        };
        this.lastUpdate = Date.now();
        this.latestData = {
            temperature: null,
            humidity: null,
            timestamp: null
        };
        this.setupGradients();
        this.initChart();
        this.startAutoUpdate();
    }

    setupGradients() {
        // Temperature gradient
        this.tempGradient = this.ctx.createLinearGradient(0, 0, 0, 400);
        this.tempGradient.addColorStop(0, 'rgba(255, 99, 132, 0.2)');
        this.tempGradient.addColorStop(1, 'rgba(255, 99, 132, 0)');

        // Humidity gradient
        this.humidGradient = this.ctx.createLinearGradient(0, 0, 0, 400);
        this.humidGradient.addColorStop(0, 'rgba(54, 162, 235, 0.2)');
        this.humidGradient.addColorStop(1, 'rgba(54, 162, 235, 0)');
    }

    initChart() {
        Chart.defaults.font.family = "'Inter', sans-serif";

        this.chart = new Chart(this.ctx, {
            type: 'line',
            data: {
                labels: this.chartData.labels,
                datasets: [
                    {
                        label: 'Temperature °C',
                        data: this.chartData.temperatures,
                        borderColor: 'rgb(255, 99, 132)',
                        backgroundColor: this.tempGradient,
                        borderWidth: 2,
                        tension: 0.4,
                        fill: true,
                        pointRadius: 4,
                        pointHoverRadius: 6,
                        pointBackgroundColor: 'rgb(255, 99, 132)',
                        pointBorderColor: '#fff',
                        pointBorderWidth: 2,
                        pointHoverBorderWidth: 2,
                        pointHoverBackgroundColor: 'rgb(255, 99, 132)',
                        pointHoverBorderColor: '#fff'
                    },
                    {
                        label: 'Humidity %',
                        data: this.chartData.humidities,
                        borderColor: 'rgb(54, 162, 235)',
                        backgroundColor: this.humidGradient,
                        borderWidth: 2,
                        tension: 0.4,
                        fill: true,
                        pointRadius: 4,
                        pointHoverRadius: 6,
                        pointBackgroundColor: 'rgb(54, 162, 235)',
                        pointBorderColor: '#fff',
                        pointBorderWidth: 2,
                        pointHoverBorderWidth: 2,
                        pointHoverBackgroundColor: 'rgb(54, 162, 235)',
                        pointHoverBorderColor: '#fff'
                    }
                ]
            },
            options: {
                responsive: true,
                maintainAspectRatio: false,
                interaction: {
                    intersect: false,
                    mode: 'index'
                },
                plugins: {
                    legend: {
                        position: 'top',
                        align: 'end',
                        labels: {
                            boxWidth: 12,
                            boxHeight: 12,
                            usePointStyle: true,
                            pointStyle: 'circle',
                            padding: 20,
                            font: {
                                size: 13,
                                weight: '600'
                            }
                        }
                    },
                    tooltip: {
                        backgroundColor: '#fff',
                        titleColor: '#1e293b',
                        bodyColor: '#475569',
                        titleFont: {
                            size: 14,
                            weight: '600'
                        },
                        bodyFont: {
                            size: 13
                        },
                        padding: 12,
                        borderColor: '#e2e8f0',
                        borderWidth: 1,
                        displayColors: true,
                        callbacks: {
                            label: function(context) {
                                const label = context.dataset.label.split(' ')[0];
                                const value = context.parsed.y;
                                const unit = context.dataset.label.split(' ')[1];
                                return `${label}: ${value.toFixed(1)}${unit}`;
                            }
                        }
                    }
                },
                scales: {
                    y: {
                        min: 0,
                        max: 100,
                        grid: {
                            color: '#e2e8f0',
                            drawBorder: false
                        },
                        ticks: {
                            padding: 10,
                            color: '#64748b',
                            font: {
                                size: 11
                            }
                        }
                    },
                    x: {
                        grid: {
                            display: false
                        },
                        ticks: {
                            padding: 10,
                            color: '#64748b',
                            font: {
                                size: 11
                            },
                            maxRotation: 0
                        }
                    }
                },
                animation: {
                    duration: 750,
                    easing: 'easeInOutQuart'
                }
            }
        });
    }

    startAutoUpdate() {
        setInterval(() => {
            if (this.latestData.temperature !== null && this.latestData.humidity !== null) {
                this.updateData(this.latestData.temperature, this.latestData.humidity, this.latestData.timestamp);
            }
        }, this.updateInterval);
    }

    setLatestData(temperature, humidity, timestamp) {
        this.latestData = { temperature, humidity, timestamp };
    }

    updateData(newTemp, newHumid, timestamp) {
        this.chartData.temperatures.shift();
        this.chartData.temperatures.push(newTemp);
        this.chartData.humidities.shift();
        this.chartData.humidities.push(newHumid);
        this.chartData.labels.shift();
        this.chartData.labels.push(timestamp);

        this.chart.data.labels = this.chartData.labels;
        this.chart.data.datasets[0].data = this.chartData.temperatures;
        this.chart.data.datasets[1].data = this.chartData.humidities;
        this.chart.update('none');
    }

    getLatestTemperature() {
        return this.latestData.temperature;
    }

    getLatestHumidity() {
        return this.latestData.humidity;
    }
}
