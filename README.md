# ISOTP BLE Bridge

The goal of this project is to build a native Macchina A0 (or ESP32 clone) firmware which can bridge BLE to ISOTP. The A0 has become increasingly more difficult to find in stock as of 2022, an alternative is to purchase the individual dev boards and assemble your own "clone".

Download and flash precompiled firmware: <br>
[https://github.com/Switchleg1/esp32-isotp-ble-bridge/releases/tag/v1.03](http://simos.app/dongle/firmware)

# Supported software

1) simos.app, iOS ecu and tcu logging, flashing, coding and adaptions. <br>
http://simos.app

2) SimosTools, android based ecu flashing and logging software: <br>
https://play.google.com/store/apps/details?id=com.app.simostools<br>

3) VW_Flash, python based ecu flashing and logging software: <br>
https://github.com/bri3d/VW_Flash<br>

4) Some other J2534 software

# Raw CAN mode

In addition to the ISO-TP bridge, the firmware supports an opt-in **raw CAN mode** for
sending/receiving raw 8-byte CAN frames (including extended 29-bit IDs) and promiscuous
bus sniffing — used to talk to non-ISO-TP channels such as a VW EPS CCP/XCP measurement
channel. It defaults OFF and does not affect ISO-TP/flashing. See
[RAW_CAN_MODE.md](RAW_CAN_MODE.md) for the exact BLE wire protocol.

# Supported hardware

1) simos.app dongle <br>
http://simos.app/dongle

2) Genuine Macchina A0 <br>
https://www.macchina.cc/catalog/a0-boards/a0-under-dash <br>

3) AMAleg - DIY Macchina A0 clone <br>
https://github.com/Switchleg1/AMAleg
