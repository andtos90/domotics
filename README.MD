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

#FRP
## Installation

### Executables
Find the latest FRP version download link [here](https://github.com/fatedier/frp/releases) and  
the executable into the `/usr/bin` folder, be carefull that `frpc` and `frps` are two different executables: 

- HASS
``` 
wget DOWNLOAD_LINK
uncompress FRPS_DOWNLOADED_FILE
cd FRPS_EXTRACTED_DIRECTORY
sudo mv frpc /usr/bin/frpc
```

- Server
```
wget DOWNLOAD_LINK
uncompress FRPS_DOWNLOADED_FILE
cd FRPS_EXTRACTED_DIRECTORY
sudo mv frps /usr/bin/frps
```

### Configurations:
My FRP configuration can be found inside the `frp` folder.

You can edit the configuration with this command:
- HASS
``` 
sudo nano /usr/share/frp/frpc.ini
```
- Server
```
sudo nano /usr/share/frp/frps.ini
```

Rember to restart FRP service with:
```
sudo systemctl restart frp
```

### Service
Create the FRP service so we can easily run FRP at the startup, you can find the service configuration
inside the `frp/services` directory. 

- HASS
``` 
cd /etc/systemd/system
sudo mv frp.service.client frp.service
```

- Server
```
cd /etc/systemd/system
sudo mv frp.service.server frp.service
```

Rember to enable FRP service at startup with:
```
sudo systemctl enable frp
```


# Ping
- crontab -e
*/3 * * * * curl -fsS --retry 3 https://hc-ping.com/XXXXXX > /dev/null

# FTP Server
https://www.digitalocean.com/community/tutorials/how-to-set-up-vsftpd-for-a-user-s-directory-on-ubuntu-18-04 (No SSL)
