const mqtt = require('mqtt');
const SensorData = require('../models/sensorModel');
require('dotenv').config();

const initMQTT = () => {
    const options = {
        username: 'Flame_Detection_System',
        password: '123',
        clean: true
    }
    const client = mqtt.connect('mqtt://mqtt-broker:1883', options);

    const TOPIC_FLAME = "yolo_uno/sensors/flame"
    const TOPIC_SMOKE = "yolo_uno/sensors/smoke"

    client.on('connect', () => {
        console.log('Backend is connecting to Broker (PORT 1883)');

        client.subscribe([TOPIC_FLAME, TOPIC_SMOKE], (err) => {
            if (!err) {
                console.log('Listening data from smoke and flame sensors');
            }
        });
    });

    client.on('message', async (topic, message) => {
        try {
            const payload = JSON.parse(message.toString());
            console.log('Recieved data');
            //const rawData = message.toString();
            //console.log(`📩 [${topic}] -> Payload:`, rawData);

            if (topic === TOPIC_SMOKE) {
                const sensors = ['mq2_1', 'mq2_2'];
                for (const sName of sensors) {
                    if (payload[sName]) {
                        const vals = payload[sName];
                        await SensorData.create({
                            topic: topic,
                            sensor_name: sName,
                            main_value: vals.SMOKE,
                            details: vals,
                        });
                    }
                }
            } else if (topic === TOPIC_FLAME) {
                if (payload.flame_data) {
                    const f = payload.flame_data;
                    await SensorData.create({
                        topic: topic,
                        sensor_name: 'Flame_sensor',
                        main_value: Math.min(f.FLAME1, f.FLAME2),
                        details: f,
                    });
                }
            } 
        } catch (error) {
            console.error('Error format of JSON or DB:', error.message);
        }
    });
};

module.exports = initMQTT;