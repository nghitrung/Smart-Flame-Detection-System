const mqtt = require('mqtt');
const SensorData = require('../models/sensorModel');
const crypto = require('crypto')
require('dotenv').config();

const AES_KEY = Buffer.from([ 0x2B, 0x7E, 0x15, 0x16, 0x28, 0xAE, 0xD2, 0xA6,
                   0xAB, 0xF7, 0x15, 0x88, 0x09, 0xCF, 0x4F, 0x3C ]);
const AES_IV = Buffer.from([ 0x28, 0x34, 0xA5, 0xAF, 0xBE, 0xB8, 0x14, 0xF5,
                         0x08, 0xE1, 0x24, 0xD2, 0xB3, 0xAB, 0xDB, 0xCE ]);

function decryptData(ciphertext) {
    try {
        if (!ciphertext) return null;

        const decipher = crypto.createDecipheriv('aes-128-cbc', AES_KEY, AES_IV);

        decipher.setAutoPadding(false);

        let decrypted = decipher.update(ciphertext, 'base64', 'utf-8');
        decrypted += decipher.final('utf-8');

        return decrypted.replace(/\0+$/g, "").trim();
    } catch (error) {
        console.error('Decrypted Error:', error.message);
        return null;
    }
}

const initMQTT = () => {
    const options = {
        username: 'Flame_Detection_System',
        password: '123',
        clean: true
    }
    const client = mqtt.connect('mqtt://mqtt-broker:1883', options);

    const TOPIC_ENCODED = "yolo_uno/sensors/all";

    client.on('connect', () => {
        console.log('Backend is connecting to Broker (PORT 1883)');

        client.subscribe(TOPIC_ENCODED, (err) => {
            if (!err) {
                console.log(`Listening for ENCODED data on ${ TOPIC_ENCODED }`);
            }
        });
    });

    client.on('message', async (topic, message) => {
        try {
            const payload = JSON.parse(message.toString());
            console.log('Recieved data');

            const smokeVal = decryptData(payload.smoke_enc_value);
            const flameVal = decryptData(payload.flame_enc_value);
            const tempVal = decryptData(payload.temp_enc_value);

            if (smokeVal !== null && flameVal !== null && tempVal !== null) {
                console.log(`Decrypted Data: Smoke = ${ smokeVal }, Flame = ${ flameVal }, Temp = ${ tempVal }`);

                await SensorData.create({ sensor_name: "Smoke Sensor", main_value: parseFloat(smokeVal) });
                await SensorData.create({ sensor_name: "Flame Sensor", main_value: parseFloat(flameVal) });
                await SensorData.create({ sensor_name: "Temp Sensor", main_value: parseFloat(tempVal) });
            } else {
                console.warn("Decrypting Failed");
            }
        } catch (error) {
            console.error('Error format of JSON or DB:', error.message);
        }
    });
};

module.exports = initMQTT;