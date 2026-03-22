# 🖥️BACKEND SYSTEM
The backend serves as the central hub, bridging the gap between ESP32 IoT devices (via MQTT) and the Frontend Dashboard (via REST API). It ensures real-time data processing, persistent storage, and system reliability.

## Technology Stack
- Runtime: Node.js (v18-alpine)
- Database ORM: Sequelize (MySQL)
- Communication: MQTT (Eclipse Mosquitto)
- Environment: Docker & Docker Compose
- Process Management: Nodemon (for development)
  
## Folder Structure
backend/
├── src/
│   ├── config/
│   │   └── database.js    
│   ├── models/
│   │   └── sensorModel.js  
│   ├── services/
│   │   └── mqttService.js  
│   └── app.js             
├── .env                   
├── Dockerfile             
├── package.json           
└── README.md

## Ports:
| Service name | Port | Protocol 
| --- | --- | --- |
| Backend API | 5000 | HTTP
| MYSQL DB | 3306 | TCP/IP
| MQTT Broker | 1883 | TCP

## API Endpoints (Base URL)
- Local Sever: [https://localhost:5000](https://localhost:5000)
