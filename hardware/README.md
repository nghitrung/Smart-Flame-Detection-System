# Flame Detection System

## Requirement 
1. Docker
2. WSL2 
3. Mosquitto

## Mosquitto Setup Guide 
1. Download and install via [here](https://mosquitto.org/). <br>
2. Add the location of the Mosquitto folder to the environment. <br>
3. Check the env via the command below: <br> 
```
mosquitto -h
```

## How to install Docker
Check out and follow step by step at [here](https://docs.docker.com/desktop/release-notes/)


### Setup MQTT via Docker
1. Clone the repository <br>
2. Move to the mqtt folder <br>
3. Run the command line: <br>
```
docker compose up -d
```
4. Check the IPv4 via the command line in Linux:
```
hostname -I
```
