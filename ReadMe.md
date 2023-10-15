## Variable power supply with USB-C PPS - PicoPD

The code is to read in 2 potentiometers, request votlage/current from USB-PD charger, and output data via an OLED scren.

The star of the design is the AP33772 USB PD controller. Big thanks to the owner of these libraries:

+ Pico-core: Earle F. Philhower
+ jrullan/Neotimer@^1.1.6
+ adafruit/Adafruit SSD1306@^2.5.7​
+ robtillaart/RunningAverage@^0.4.

The code should work out of the box with [PicoPD board](https://www.tindie.com/products/31724/) designed by CentyLab. However you can replicate with any micro-controller with AP33772 evaluation board. Make sure to change your I2C bus configuration to match with the pin out.