# Controllino
- rename writeMqttCredentials.sample.ino to writeMqttCredentials.ino, edit with your credentials, upload and run the sketch once
- upload and run the controllino MqttLight sketch

# Configurator
- install gitpython with pi user: pip3 install gitpython 
- in homeassistant root: ./configurator.py configurator.conf

# Dash Button
- install http://docs.nekmo.org/amazon-dash/ with:
```
sudo pip3 install -U amazon_dash
sudo python3 -m amazon_dash.install
```
- edit the configuration in /etc/amazon-dash.yml
- sudo systemctl enable amazon-dash
- run`sudo systemctl edit amazon-dash` with:
```
[Service]
Environment="LANG=C.UTF-8"
Environment="LANG=C.UTF-8"
```

# Home assistant configuration synced with git
I use GIT sparse checkout to sync only the .homeassistant directory (https://stackoverflow.com/a/13738951) 
