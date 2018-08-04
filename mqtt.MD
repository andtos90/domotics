brew services start mosquitto
mosquitto_sub -h localhost -t /house/light/status
mosquitto_pub -h localhost -t /house/light/command -m toggleLight