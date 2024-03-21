# ArduinoSnake
by e4rdx

## Description
This is a simple snake game for Arduino. It utilizes a monochrome 128x64 OLED display and a joystick for input. The game is written in C++ and is based on the Adafruit SSD1306 library.

## Intended Hardware
- Microcontroller: Seeeeduino XIAO
- Display: SBC-OLED01 0,96" OLED Display - F11-1U0 DEBO OLED2 0.96 9296686-219
- JoyStick: Generic Analog 2-Axis Thumb Joystick with Select Button
- Buzzer: Generic Active Buzzer Module

## Wiring
- Display:
  - VCC -> 3.3V
  - GND -> GND
  - SCL -> SCL (D5)
  - SDA -> SDA (D4)
- Joystick:
  - VCC -> 3.3V
  - GND -> GND
  - SW -> D1
  - VRx -> A2
  - VRy -> A3
- Buzzer:
  - VCC -> 3.3V
  - GND -> GND
  - SIG -> A0

## Installation
1. Clone the repository
2. Open in your preferred IDE
3. Install platformIO using pip: `pip install platformio`
4. (Optional) Install the platformIO IDE and Arduino extension for your preferred IDE
5. Build the project using platformIO: `pio run`
6. If you are not using the Seeeduino XIAO, you will need to change the board in the platformio.ini file. Look your specific board up in the platformIO documentation and change the line `board = seeeduino_xiao` and the environment to your board's name.
7. Compile for production and upload to your board: `pio run -t upload`

## Troubleshooting
- If components are not working, check the wiring and the pinout in the code.
- If the display is not working, check the I2C address of the display and change it in the code if necessary.
- If the upload to your microcontroller fails, check the USB connection, the set usb port and the board settings in the platformio.ini file.
If that doesn't help, enter the bootloader mode of your microcontroller and try again. With the Seeeduino XIAO, you can do this by shorting the two reset pins twice in quick succession.
The LED on the board should start fading on and off and the board should show up as a new USB device. If it doesn't, try again.
- If you are experiencing other issues, please look at the platformIO documentation or the documentation of your specific microcontroller.