## GEENIE Board Firmware

This repository contains all firmware files for the GEENIE EEG board.

##### Board Flashing

To flash the firmware open the project using the [PlatformIO](https://platformio.org/) add-on in [Visual Studio Code](https://code.visualstudio.com/) 
and use the environment which uses the **featheresp32** board and **arduino** framework. 

The project settings are set in [platformio.ini](platformio.ini)

##### USB Driver

A [Huzzah32](https://www.adafruit.com/product/3405) feather ESP32 board should be connected to the PC using a USB cable. It is not necessary for the ESP
to be connected to the GEENIE board

In Linux the board should be immediately recognized. To check use:
```bash
ls /dev/tty*
``` 
before and after connecting the board to the pc.

A new entity under `/dev/ttyUSB0` or `/dev/ttyUSB1` should be present.

In Windows the device should be visible under a USB COM port (i.e. `COM6`) in Device Manager.
If it is not recognized by the system try installing the CP210x USB Driver:

Link: [https://www.silabs.com/developers/usb-to-uart-bridge-vcp-drivers](https://www.silabs.com/developers/usb-to-uart-bridge-vcp-drivers)

##### Monitoring

To monitor the device after flashing, use a serial monitor in Visual Studio or any other Serial Monitor program.
Note that the monitor BaudRate should be identical to the device BaudRate (i.e. 115200) which is specified
in [platformio.ini](platformio.ini).

##### Data Visualization

After the huzzah32 board is flashed, attach it to the GEENIE board if not already attached.

To visualize the data in real time, build the Brainflow Library from the custom **geenie** branch,
install the Python library from the local build files and run the files in the python examples folder of 
Brainflow. 

More instructions in the forked Brainflow repository.