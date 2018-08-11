Example without password:

brew services start mosquitto
mosquitto_sub -h localhost -t /house/light/status
mosquitto_pub -h localhost -t /house/light/command -m toggleLight

mosquitto_sub -h 192.168.1.168 -p 1883 -u homeassistant -t /house/light/status