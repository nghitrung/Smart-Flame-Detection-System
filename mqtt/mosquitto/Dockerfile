FROM eclipse-mosquitto:latest

COPY config/ /mosquitto/config

# encode password
RUN mosquitto_passwd -U /mosquitto/config/pwfile

RUN chown -R mosquitto:mosquitto /mosquitto
