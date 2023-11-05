# ESP32 led dimmer timer

## About the project

The project started out as an idea to copy the behaviour of a bedside clock my friends used, which simulated the rising of the sun with a dimmable LED-light.

To make the effect more realistic, it took a while to get it right, as my knowledge of coding was little and experience with embedded systems even less.

*Pardon my french*, if the code is a bit simplistic and not too polished as I've worked on this on my free time and it's a personal hobby-project. This is my first public project and also my first ever project with an embedded system. Nevertheless, I've found the simulation of a sunrise to help make my mornings much more cheerful during the long and dark winter mornings in Estonia. Maybe someone else needs something exactly like it.

The result is a simple project that uses:
* ESP32 (ESP32-WROOM32 or ESP32-C3 Supermini, but probably any ESP32 devkit will do)
* N-mosfet transistor IRLB8721 for PWM dimming
* DC-DC 3A Step-down Power Supply MP1584RN
* 2A 12V Adapter
* 12V LED-strip
* A small tactile switch 

And accomplishes:
* A simple locally accessible website, where timers can be created to start and stop fade-in and fade-out.
* A button to trigger LED fade-in and fade-out manually
* A locally accessible API endpoint to trigger LED fade-in and fade-out manually

## How to use

I used VS Code and PlatformIO to handle the project and it's dependencies.

### Power in

Connect the 12V Adapter to the Step-down power supply's input and set the output to 5V or 3.3V depending on which voltage pin you intend to use.

Connect the outputs of the power supply to 3.3V (+) or 5V (+) and GND (-) depending on which voltage you chose.

Connect the 12V positive (+) with the LED-strip's positive (+) wire.

### ESP32

#### Network

**Set your WiFi credentials here.** You can add multiple as the project is using WiFiMulti. The project is not usable without internet connection currently. You are welcome to add possibility to set the time manually, without internet connection.

```cpp
const char *ssid_1 = "addWiFiSSIDHere"
const char *pw_1 = "addWiFiPasswordHere"
```
**Set your preferred domain name** for the ESP32 under `mDnsDomainName`. This will be used as the url, where you can access the website to create the timers. By default it's `esp32led.local`

```cpp
char * mDnsDomainName = "esp32led"
```

**Set your preferred soft-AP credentials here**. You can directly connect to the the WiFi-network created by the ESP32 to access the website and create the timers.

```cpp
const char* ap_ssid = "esp32led";
const char* ap_password = "password";
```

#### Pinout

Find a suitable GPIO pin that supports PWM (*Pulse Width Modulation*) and connect it to the **transistor's Gate**. By default, the pin use is GPIO_5, which worked for ESP32-C3 Supermini. GPIO_23 worked on ESP32-WROOM32. Any GPIO pin should be PWM capable on ESP32 devkits, but check your board's datasheet to be sure.

If you need to change the pin for PWM, you can do it in

```cpp
const int pwmOutput = 5;
```

If you're using a tactile switch to manually trigger the LED, you can either use GPIO_10 on ESP32-C3 Supermini or any other pin that supports pullup resistor functionality. You can change the pin in

```cpp
const uint8_t ledButtonPin = 10
```

If you decide to use ESP32's capacitative touch sensor, you need to uncomment `touchLedOnOff()` and add it into `loop()`. By default the touchread checks GPIO_13. As far as I understand, capacitative touch is not possible on ESP32-C3. You can edit the pin in

```cpp
int t = touchRead(13);
```

### Transistor IRLB8721

This transistor is an N-mosfet transistor, which allows the output voltage to be controlled by Pulse Width Modulation. 

It has three pins called the Gate, Drain and Source (left to right when looked from front). As said before, you need to connect the ESP32's GPIO pin to the **Gate** of the transistor.

Connect the **Drain** of the transistor to the the negative (-) wire of the LED. 

Connect the **Source** of the transistor to the GND pin of the ESP32.


### LED-strip

As said before, the positive (+) wire of the LED needs to connect to the 12V input from the power adapter and the negative (-) wire to the Drain (middle pin) of the transistor.

Please do make sure that the system can handle the power requirements of the LED-strip. According to my calculations, an LED-strip with power consumption of 14.4W/meter can have a maximum length of ~1,6m, before it starts using more than 2A of current.

To calculate this, you can use Ohm's law:

`current (A) = power (W) / voltage (V)` 

If your adapter can handle 2A of current at 12V, it means that the maximum power required is `W = A * V = 2A * 12V = 24W`. If you know the power consumption of your LED strip (Watts per meter), you can simply divide the calculated max power with LED's power consumption:

`24W / 14,4W/m = ~1,6m`

## The locally accessible website

To set the timers, go to `esp32led.local` (or whatever you named it at `mDnsDomainName`), once the ESP32 is connected to your WiFi or you've connected to it's soft-AP. You'll see a simple UI, where you can set the start and end times for fadein (ON) and fadeout (OFF). The led will fade in and out for the difference of start and end times in minutes. If you set ON start 12:05 and end 12:15, the fadein will last for 10 minutes, increasing the brightness of the LED once every millisecond.

There are two buttons - Save and Switch. Save will store the current selected on and off times in a file on the flash of the ESP32. Switch will fade the LED in or out in 1.8 seconds (0.03 minutes).

As per `TimeAlarms` library, up to 6 alarms can be used at the same time, but I've created more at once, but not seen them in action.

## Final words

I encourage you to do your own research of the topics mentioned here. I am only a hobbyist and a beginner at that. The current implementation has worked for me and I hope that it will work for you as well.

### Shoutout to used libraries

```yml
adafruit/Adafruit GFX Library@^1.11.3
adafruit/Adafruit SSD1306@^2.5.7
arduino-libraries/NTPClient@^3.2.1
paulstoffregen/Time@^1.6.1
paulstoffregen/TimeAlarms@0.0.0-alpha+sha.c291c1ddad
ottowinter/ESPAsyncWebServer-esphome@^3.0.0
me-no-dev/AsyncTCP@^1.1.1
arduino-libraries/Arduino_JSON@^0.2.0
```