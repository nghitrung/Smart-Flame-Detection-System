const { DataTypes } = require('sequelize');
const sequelize = require('../config/database');

const SensorData = sequelize.define('SensorData', {
    sensor_name: { type: DataTypes.STRING, allowNull: false },
    main_value: { type: DataTypes.FLOAT },
});

module.exports = SensorData