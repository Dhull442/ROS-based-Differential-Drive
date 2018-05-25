/*
  Example sketch for the PS3 USB library - developed by Kristian Lauszus
  For more information visit my blog: http://blog.tkjelectronics.dk/ or
  send me an e-mail:  kristianl@tkjelectronics.com
*/

#include <PS3USB.h>

// Satisfy the IDE, which needs to see the include statment in the ino too.
#ifdef dobogusinclude
#include <spi4teensy3.h>
#endif
#include <SPI.h>

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

USB Usb;
/* You can create the instance of the class in two ways */
PS3USB PS3(&Usb); // This will just create the instance
//PS3USB PS3(&Usb,0x00,0x15,0x83,0x3D,0x0A,0x57); // This will also store the bluetooth address - this can be obtained from the dongle when running the sketch
/*
  bool printAngle;
  uint8_t state = 0;*/
double lx, ly, rx, ry, l2, r2, max2 = 255;
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
  Serial.print(F("\r\nPS3 USB Library Started"));
  K1.start();
  K1.home().wait();
  K2.start();
  K2.home().wait();
}
void loop() {
  Usb.Task();

  /* if (PS3.PS3Connected || PS3.PS3NavigationConnected) {
     if (PS3.getAnalogHat(LeftHatX) > 137 || PS3.getAnalogHat(LeftHatX) < 117 || PS3.getAnalogHat(LeftHatY) > 137 || PS3.getAnalogHat(LeftHatY) < 117 || PS3.getAnalogHat(RightHatX) > 137 || PS3.getAnalogHat(RightHatX) < 117 || PS3.getAnalogHat(RightHatY) > 137 || PS3.getAnalogHat(RightHatY) < 117) {
       Serial.print(F("\r\nLeftHatX: "));
       Serial.print(PS3.getAnalogHat(LeftHatX));
       Serial.print(F("\tLeftHatY: "));
       Serial.print(PS3.getAnalogHat(LeftHatY));
       if (PS3.PS3Connected) { // The Navigation controller only have one joystick
         Serial.print(F("\tRightHatX: "));
         Serial.print(PS3.getAnalogHat(RightHatX));
         Serial.print(F("\tRightHatY: "));
         Serial.print(PS3.getAnalogHat(RightHatY));

      }
     }*/
  if (PS3.PS3Connected || PS3.PS3NavigationConnected) {
    lx = (double)PS3.getAnalogHat(LeftHatX);
    // ly = PS3.getAnalogHat(LeftHatY);
    // rx = PS3.getAnalogHat(RightHatX);
    ry = (double)PS3.getAnalogHat(RightHatY);
    ry = 128 - ry;
  }
  // Analog button values can be read from almost all buttons
  l2 = (double) PS3.getAnalogButton(L2);
  r2 = (double)PS3.getAnalogButton(R2);
  if (PS3.getButtonClick(PS))
    Serial.print(F("\r\nPS"));
  spd1 = (double)(K1.getMax().value() - K1.getMin().value()) / 40 ;
  spd2 = (double)(K2.getMax().value() - K2.getMin().value()) / 40 ;
  spd1m = 0;
  spd2m = 0;
  spdfactor = spd2 / spd1;
  kangaroo_speed_direction();
  /* if (PS3.getButtonClick(TRIANGLE))
     Serial.print(F("\r\nTraingle"));
    if (PS3.getButtonClick(CIRCLE))
     Serial.print(F("\r\nCircle"));
    if (PS3.getButtonClick(CROSS))
     Serial.print(F("\r\nCross"));
    if (PS3.getButtonClick(SQUARE))
     Serial.print(F("\r\nSquare"));

    if (PS3.getButtonClick(UP)) {
     Serial.print(F("\r\nUp"));
     PS3.setLedOff();
     PS3.setLedOn(LED4);
    }
    if (PS3.getButtonClick(RIGHT)) {
     Serial.print(F("\r\nRight"));
     PS3.setLedOff();
     PS3.setLedOn(LED1);
    }
    if (PS3.getButtonClick(DOWN)) {
     Serial.print(F("\r\nDown"));
     PS3.setLedOff();
     PS3.setLedOn(LED2);
    }
    if (PS3.getButtonClick(LEFT)) {
     Serial.print(F("\r\nLeft"));
     PS3.setLedOff();
     PS3.setLedOn(LED3);
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
    if (printAngle) {
     Serial.print(F("\r\nPitch: "));
     Serial.print(PS3.getAngle(Pitch));
     Serial.print(F("\tRoll: "));
     Serial.print(PS3.getAngle(Roll));
    }
    }
    else if (PS3.PS3MoveConnected) { // One can only set the color of the bulb, set the rumble, set and get the bluetooth address and calibrate the magnetometer via USB
    if (state == 0) {
     PS3.moveSetRumble(0);
     PS3.moveSetBulb(Off);
    } else if (state == 1) {
     PS3.moveSetRumble(75);
     PS3.moveSetBulb(Red);
    } else if (state == 2) {
     PS3.moveSetRumble(125);
     PS3.moveSetBulb(Green);
    } else if (state == 3) {
     PS3.moveSetRumble(150);
     PS3.moveSetBulb(Blue);
    } else if (state == 4) {
     PS3.moveSetRumble(175);
     PS3.moveSetBulb(Yellow);
    } else if (state == 5) {
     PS3.moveSetRumble(200);
     PS3.moveSetBulb(Lightblue);
    } else if (state == 6) {
     PS3.moveSetRumble(225);
     PS3.moveSetBulb(Purple);
    } else if (state == 7) {
     PS3.moveSetRumble(250);
     PS3.moveSetBulb(White);
    }

    state++;
    if (state > 7)
     state = 0;
    delay(1000);
    }*/
}
void kangaroo_speed_direction()
{ double rotfac;
  if (l2 == 0)
  { spd1m = spd1 / 5;
    spd2m = spd2 / 5;
    rotfac = r2 / max2;
    spd1m *= rotfac;
    spd2m *= -rotfac;
  }
  if (r2 == 0)
  { spd1m = spd1 / 5;
    spd2m = spd2 / 5;
    rotfac = -l2 / max2;
    spd1m *= rotfac;
    spd2m *= -rotfac;
  }
  double fac = 0;
  spd1 *= (ry / 128);
  spd2 *= (ry / 128);
  lx -= 128;
  fac = lx / 128;
  if( fac >= 0)
  {spd1 = (1 + fac*fac) / 2 * spd1 + spd1m ;
  spd2 = (1 - fac*fac) / 2 * spd2 + spd2m ;}
  else if(fac < 0)
  { spd1 = (1 - fac*fac) / 2 * spd1 + spd1m ;
  spd2 = (1 + fac*fac) / 2 * spd2 + spd2m ;    
    }
  K1.s(spd1).wait();
  K2.s(spd2).wait();

}
