A simple WebSocket Server & Arduino Sketch.
Connects a wifi-enabled (ESP8266) Arduino-nano-like board to an Express server via WebSocket.

Setup Steps ---

    1.  Load sketch onto ESP8266 (adding wifi info, server IP, and server port number).
        If running locally, your server IP will be your ethernet or wifi IP found in your PC/Mac's network settings.
        The port defaults to 3000, but can be changed in index.js.

    2.  Run index.js using node. "node index.js"

    3.  Open a web browser and navigate to "localhost:3000".
        This will open the Vue app. It currently doesn't do anything.

    3.  The Arduino Sketch has more info for setting up the ESP8266.
        Upload the sketch. If Wifi credentials and IP were entered correctly, it'll connect.
        Ground Pin D2 to send a message to the server.

Using the ESP8266 board with Arduino IDE:

    Use this Board Manager URL: https://arduino.esp8266.com/stable/package_esp8266com_index.json (added under File > preferences > Settings tab)
    Select 'Generic ESP8266 Module' from the board selector.
