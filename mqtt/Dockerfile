FROM eclipse-mosquitto:latest

COPY mosquitto/config/ /mosquitto/config

RUN mkdir -p /mosquitto/log /mosquitto/data

RUN chown -R mosquitto:mosquitto /mosquitto

RUN chmod 0600 /mosquitto/config/pwfile

# encode password
RUN mosquitto_passwd -U /mosquitto/config/pwfile


