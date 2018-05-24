/*
  Example sketch for the PS3 Bluetooth library - developed by Kristian Lauszus
  For more information visit my blog: http://blog.tkjelectronics.dk/ or
  send me an e-mail:  kristianl@tkjelectronics.com
*/

#include <PS3BT.h>
#include <usbhub.h>

#include <SoftwareSerial.h>
#include <Kangaroo.h>

// Arduino TX (pin 11) goes to Kangaroo S1
// Arduino RX (pin 10) goes to Kangaroo S2
// Arduino GND         goes to Kangaroo 0V
// Arduino 5V          goes to Kangaroo 5V (OPTIONAL, if you want Kangaroo to power the Arduino)
#define TX_PIN 14
#define RX_PIN 15
// Independent mode channels on Kangaroo are, by default, '1' and '2'.

KangarooSerial  K(Serial3);
KangarooChannel K1(K, '1');
KangarooChannel K2(K, '2');

// Satisfy the IDE, which needs to see the include statment in the ino too.
#ifdef dobogusinclude
#include <spi4teensy3.h>
#endif
#include <SPI.h>

USB Usb;
//USBHub Hub1(&Usb); // Some dongles have a hub inside

BTD Btd(&Usb); // You have to create the Bluetooth Dongle instance like so
/* You can create the instance of the class in two ways */
//PS3BT PS3(&Btd); // This will just create the instance
PS3BT PS3(&Btd, 0x00, 0x15, 0x83, 0x3D, 0x0A, 0x57); // This will also store the bluetooth address - this can be obtained from the dongle when running the sketch
/*
  bool printTemperature, printAngle;*/
double lx, ly, rx, ry, l2, r2, max2;
double spd1 = 0, spd2 = 0, spd1m, spd2m;
double spdfactor;
void setup() {
  Serial.begin(115200);
  Serial3.begin(19200);
#if !defined(__MIPSEL__)
  while (!Serial); // Wait for serial port to connect - used on Leonardo, Teensy and other boards with built-in USB CDC serial connection
#endif
  if (Usb.Init() == -1) {
    Serial.print(F("\r\nOSC did not start"));
    while (1); //halt
  }
  Serial.print(F("\r\nPS3 Bluetooth Library Started"));
  //  SerialPort.listen();

  K1.start();
  K1.home().wait();
  K2.start();
  K2.home().wait();
}
void loop() {
  Usb.Task();

  if (PS3.PS3Connected || PS3.PS3NavigationConnected) {
    lx = (double)PS3.getAnalogHat(LeftHatX);
    // ly = PS3.getAnalogHat(LeftHatY);
    // rx = PS3.getAnalogHat(RightHatX);
    ry = (double)PS3.getAnalogHat(RightHatY);
    ry = 128 - ry;
    /*if (PS3.getAnalogHat(LeftHatX) > 137 || PS3.getAnalogHat(LeftHatX) < 117 || PS3.getAnalogHat(LeftHatY) > 137 || PS3.getAnalogHat(LeftHatY) < 117 || PS3.getAnalogHat(RightHatX) > 137 || PS3.getAnalogHat(RightHatX) < 117 || PS3.getAnalogHat(RightHatY) > 137 || PS3.getAnalogHat(RightHatY) < 117)
      {
      Serial.print(F("\r\nLeftHatX: "));
      Serial.print(PS3.getAnalogHat(LeftHatX));
      Serial.print(F("\tLeftHatY: "));
      Serial.print(PS3.getAnalogHat(LeftHatY));
      if (PS3.PS3Connected) { // The Navigation controller only have one joystick
        Serial.print(F("\tRightHatX: "));
        Serial.print(PS3.getAnalogHat(RightHatX));
        Serial.print(F("\tRightHatY: "));
        Serial.print(PS3.getAnalogHat(RightHatY));
      }*/
  }

  // Analog button values can be read from almost all buttons
  l2 = (double) PS3.getAnalogButton(L2);
  r2 = (double)PS3.getAnalogButton(R2);



  if (PS3.getButtonClick(PS)) {
    Serial.print(F("\r\nPS"));
    PS3.disconnect();
  }
  /*else {
    if (PS3.getButtonClick(TRIANGLE)) {
      Serial.print(F("\r\nTraingle"));
     // PS3.setRumbleOn(RumbleLow);
    }
    if (PS3.getButtonClick(CIRCLE)) {
      Serial.print(F("\r\nCircle"));
      //PS3.setRumbleOn(RumbleHigh);
    }
    if (PS3.getButtonClick(CROSS))
      Serial.print(F("\r\nCross"));
    if (PS3.getButtonClick(SQUARE))
      Serial.print(F("\r\nSquare"));

    if (PS3.getButtonClick(UP)) {
      Serial.print(F("\r\nUp"));
      if (PS3.PS3Connected) {
        PS3.setLedOff();
        PS3.setLedOn(LED4);
      }
    }
    if (PS3.getButtonClick(RIGHT)) {
      Serial.print(F("\r\nRight"));
      if (PS3.PS3Connected) {
        PS3.setLedOff();
        PS3.setLedOn(LED1);
      }
    }
    if (PS3.getButtonClick(DOWN)) {
      Serial.print(F("\r\nDown"));
      if (PS3.PS3Connected) {
        PS3.setLedOff();
        PS3.setLedOn(LED2);
      }
    }
    if (PS3.getButtonClick(LEFT)) {
      Serial.print(F("\r\nLeft"));
      if (PS3.PS3Connected) {
        PS3.setLedOff();
        PS3.setLedOn(LED3);
      }
    }

    if (PS3.getButtonClick(L1))
      Serial.print(F("\r\nL1"));
    if (PS3.getButtonClick(L3))
      Serial.print(F("\r\nL3"));
    if (PS3.getButtonClick(R1))
      Serial.print(F("\r\nR1"));
    if (PS3.getButtonClick(R3))
      Serial.print(F("\r\nR3"));

    if (PS3.getButtonClick(SELECT)) {
      Serial.print(F("\r\nSelect - "));
      PS3.printStatusString();
    }
    if (PS3.getButtonClick(START)) {
      Serial.print(F("\r\nStart"));
      printAngle = !printAngle;
    }
    }
    #if 0 // Set this to 1 in order to see the angle of the controller
    if (printAngle) {
    Serial.print(F("\r\nPitch: "));
    Serial.print(PS3.getAngle(Pitch));
    Serial.print(F("\tRoll: "));
    Serial.print(PS3.getAngle(Roll));
    }
    #endif
    }
    #if 0 // Set this to 1 in order to enable support for the Playstation Move controller
    else if (PS3.PS3MoveConnected) {
    if (PS3.getAnalogButton(T)) {
    Serial.print(F("\r\nT: "));
    Serial.print(PS3.getAnalogButton(T));
    }
    if (PS3.getButtonClick(PS)) {
    Serial.print(F("\r\nPS"));
    PS3.disconnect();
    }
    else {
    if (PS3.getButtonClick(SELECT)) {
      Serial.print(F("\r\nSelect"));
      printTemperature = !printTemperature;
    }
    if (PS3.getButtonClick(START)) {
      Serial.print(F("\r\nStart"));
      printAngle = !printAngle;
    }
    if (PS3.getButtonClick(TRIANGLE)) {
      Serial.print(F("\r\nTriangle"));
      PS3.moveSetBulb(Red);
    }
    if (PS3.getButtonClick(CIRCLE)) {
      Serial.print(F("\r\nCircle"));
      PS3.moveSetBulb(Green);
    }
    if (PS3.getButtonClick(SQUARE)) {
      Serial.print(F("\r\nSquare"));
      PS3.moveSetBulb(Blue);
    }
    if (PS3.getButtonClick(CROSS)) {
      Serial.print(F("\r\nCross"));
      PS3.moveSetBulb(Yellow);
    }
    if (PS3.getButtonClick(MOVE)) {
      PS3.moveSetBulb(Off);
      Serial.print(F("\r\nMove"));
      Serial.print(F(" - "));
      PS3.printStatusString();/
    }
    }
    if (printAngle) {
    Serial.print(F("\r\nPitch: "));
    Serial.print(PS3.getAngle(Pitch));
    Serial.print(F("\tRoll: "));
    Serial.print(PS3.getAngle(Roll));
    }
    else if (printTemperature) {
    Serial.print(F("\r\nTemperature: "));
    Serial.print(PS3.getTemperature());
    }
    }*/
  spd1 = (double)(K1.getMax().value() - K1.getMin().value()) / 100 ;
  spd2 = (double)(K2.getMax().value() - K2.getMin().value()) / 100 ;
  spd1m = 0;
  spd2m = 0;
  spdfactor = spd2 / spd1;
  kangaroo_speed_direction();
  // K1.s(speed).wait();
  // delay(5000);

  // K1.s(-speed).wait();
  // delay(5000);
  //#endif
}
void kangaroo_speed_direction()
{ double rotfac;
  if (l2 == 0)
  { spd1m = spd1;
    spd2m = spd2;
    rotfac = r2 / max2;
    spd1m *= rotfac;
    spd2m *= -rotfac;

  }
  if (r2 == 0)
  { spd1m = spd1;
    spd2m = spd2;
    rotfac = -l2 / max2;
    spd1m *= rotfac;
    spd2m *= -rotfac;
  }
  double fac = 0;
  spd1 *= (ry / 128);
  lx -= 128;
  fac = lx / 128;
  spd1 = (1 + fac) / 2 * spd1 + spd1m;
  spd2 = (1 - fac) / 2 * spd1 * spdfactor + spd2m ;
  K1.s(spd1).wait();
  K2.s(spd2).wait();

}
