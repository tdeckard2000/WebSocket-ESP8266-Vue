A simple WebSocket Server & Arduino Sketch.
Connects a wifi-enabled (ESP8266) Arduino-nano-like board to an Express server via WebSocket.

Setup Steps ---

    1.  Load sketch onto ESP8266 (adding wifi info and server).
        If running locally, 
        The port defaults to 3000, but can be changed in index.js.

    2.  Navigate to websocket-5d15bc66efcd.herokuapp.com to try it out.

    3.  If you would rather run the app locally, use this command "node index.js".
        Then navigate to localhost:3000
        You'll also need to update the ESP8266 sketch with your IP address ex. 12.34.125.10:3000
        Your server IP will be your ethernet or wifi IP found in your PC/Mac's network settings.

Using the ESP8266 board with Arduino IDE:

    Use this Board Manager URL: https://arduino.esp8266.com/stable/package_esp8266com_index.json (added under File > preferences > Settings tab)
    Select 'Generic ESP8266 Module' from the board selector.

![Bread Board Example](../public/Bread_Board_Example.jpg)