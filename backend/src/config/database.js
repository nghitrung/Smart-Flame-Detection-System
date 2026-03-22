const { Sequelize } = require('sequelize');
require('dotenv').config();

const sequelize = new Sequelize(
    process.env.DB_NAME || 'flame_detection_system',
    process.env.DB_USER || 'root',
    process.env.DB_PASS || '123',

    {
        host: process.env.DB_HOST || 'mysql-db',
        dialect: 'mysql',
        logging: false
    }
);

module.exports = sequelize;