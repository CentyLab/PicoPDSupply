/*
  Example code that will cycle through all possble PPS voltage starting at 4.2V
  Will print out current reading through the 10mOhm sense resistor and blink LED
*/

#include <Arduino.h>
#include <AP33772.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <neotimer.h>
#include "RunningAverage.h"

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
// 0X3C+SA0 - 0x3C or 0x3D
#define OLED_RESET -1       // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 oled(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

AP33772 usbpd;                          // Automatically wire to Wire0,
Neotimer mytimer_100ms = Neotimer(100); // Set timer's preset to 1s
Neotimer mytimer_500ms = Neotimer(500); // Set timer's preset to 1s
RunningAverage myRA_V(100);             // 20 element for running average
RunningAverage myRA_I(100);             // 20 element for running average

bool state = 0;
int targetVoltage;
int targetCurrent;
int oldVoltage;
int oldCurrent;

void setup()
{
  Wire.setSDA(0);
  Wire.setSCL(1);
  Wire.begin();

  Serial.begin(115200);
  delay(1000); //Delay so AP33772 has enough time to bootup
  usbpd.begin(); // Start pulling the PDOs from power supply
  usbpd.printPDO();

  if (!oled.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS))
    while (1)
    {
      Serial.println("Oled init fail");
    }

  oled.clearDisplay();
  oled.setTextSize(1);
  oled.setTextColor(SSD1306_WHITE);
  oled.setCursor(0, 0);

  oled.println(F("USB C - Power Supply"));
  oled.display();

  pinMode(25, OUTPUT); // Built in LED
  pinMode(23, OUTPUT); // Load Switch
  digitalWrite(23, HIGH);
  delay(2000);
  oled.clearDisplay();
  // oled.setRow(0);
  // oled.println("Voltage (V): ");
  // oled.setRow(1);
  // oled.println("Current (mA): ");

  pinMode(A2, INPUT);
  pinMode(A1, INPUT);
  analogReadResolution(12);

  myRA_V.clear();
  myRA_I.clear();
}

void loop()
{

  // Everyloop use rolling average to filter out noise from the pot.
  targetVoltage = map(analogRead(A2), 20, 4080, 4000, 21000); //USB-C voltage lower than 4.2V will cause the 3V3 rail to collapse
  myRA_V.addValue(targetVoltage); // Variable is redundance but display for code understanding

  targetCurrent = map(analogRead(A1), 20, 4080, 500, 5000);
  myRA_I.addValue(targetCurrent); // Variable is redundance but display for code understanding

  if (mytimer_500ms.repeat()) //Reduce spam call over CC1/CC2
  {
    usbpd.setVoltage(int(myRA_V.getAverage()));    // Casting Float -> Int
    usbpd.setMaxCurrent(int(myRA_I.getAverage())); // Casting Float -> Int
  }

  if (mytimer_100ms.repeat()) //Reduce refresh rate of OLED
  {
    oled.clearDisplay();
    oled.setCursor(0, 0);
    oled.print("Voltage (V): ");
    oled.println(usbpd.readVoltage() / 1000.0);

    oled.write("Current (mA): ");
    oled.println(usbpd.readCurrent());

    oled.write("Target current: ");
    oled.println(targetCurrent);

    oled.display();
  }
}
