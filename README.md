# Real-Time Slack Pager on ESP8266 and SSD1306 OLED display

This sketch turns your ESP chip into real time Slack pager, dedicated display for Slack channels. 

**Demo video** : https://youtu.be/F2sbvJaTipA

Before running this Sketch, make sure the set the following constants in the program:

* `SLACK_TOKEN` - The API token of your slack bot (get it at https://my.slack.com/services/new/bot)
* `WIFI_SSID` - Your WiFi network name (SSID)
* `WIFI_PASSWORD` - Your WiFi password

In addition, you will need to install the following Arduino libraries:
- [WebSockets](https://github.com/Links2004/arduinoWebSockets), 
- [ArduinoJson](https://github.com/bblanchon/ArduinoJson),
- [ESP8266-oled-ssd1306](https://github.com/squix78/esp8266-oled-ssd1306)

I used NodeMCU ESP board for this code (available for less than $10 at http://amzn.to/29v9AaZ) and SSD1306 (available for $10 at http://amzn.to/2kKtu7w). You can also buy these for a lot cheaper if you can wait a couple weeks: NodeMCU for $5.26 at http://www.banggood.com/NodeMcu-Lua-WIFI-Internet-Things-Development-Board-Based-ESP8266-CP2102-Wireless-Module-p-1097112.html?p=NR1115712752201408A0 and OLED screen for $5.49 at http://www.banggood.com/0_96-Inch-4Pin-White-IIC-I2C-OLED-Display-Module-12864-LED-For-Arduino-p-958196.html?p=NR1115712752201408A0

TODO : 

- [ ] When initial JSON is received from Slack, create a hash table of channel IDs/their names and also user IDs/usernames so that you could see what channel the message is coming from
- [ ] Connect Neopixels and specify certain triggers for various colors
- [ ] Enable a way to specify what channel you want to listen in, instead of all channels where the bot is present

Copyright (C) 2016, Maks Surguy

License: MIT.
