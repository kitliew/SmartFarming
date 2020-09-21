"SMART FARMING" with Arduino Uno and RaspberryPi 3b+ and ZeroW


Soil based smart farming

Have the potential to upgrade into Hydroponic smart farming

Arduino read the following sensors value
1. Temperature (DS18B20)
2. Humidity and Temperature (DHT22)
3. pH 
4. EC
send value to RPI0W via USB port
RPI0W collect and convert to readable data 
Send to server/database in RPI3b+ via WiFi



Ways to send data from PI0W to server(RPI3b+)
1. mysql-connector-python remote access
        pros: less scripting
        cons: have to add every client's IP address in 50-server.cnf
2. HTTP post
        pros: easy to setup (only require server IP address)
        cons: slow?
3. MQTT

# SETUP for MySQL
1. "GRANT ALL PRIVILEGES ON *.* TO 'smartfarming'@'%' IDENTIFIED BY 'smartfarming' WITH GRANT OPTION; "     # "smartfarming" as username and password, % grant access for all IP
2. "FLUSH PRIVILEGES; "
3. "CREATE DATABASE SmartFarming; "     # "SmartFarming" is db_name