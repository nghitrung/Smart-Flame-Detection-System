const express = require('express');
const dotenv = require('dotenv');
const sequelize = require('./config/database');
const initMQTT = require('./services/mqttService');

dotenv.config();

const app = express();
const PORT = process.env.PORT || 5000

app.use(express.json());

async function startServer() {
    try {
        console.log('Check the connection of DB');

        await sequelize.sync({alter: true});
        console.log('MYSQL is available!');
        initMQTT();

        app.listen(PORT, () => {
            console.log('Backend server is running');
        });

        app.get('/status', (req, res) => {
            res.json({ status: 'Online', database: 'Connected', mqtt: 'Running' });
        });
    } catch (error) {
        console.log('Can not start the system', error.message);
        process.exit(1);
    }
}

startServer();