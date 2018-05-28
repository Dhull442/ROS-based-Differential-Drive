
#include <PS3USB.h>

// Satisfy the IDE, which needs to see the include statment in the ino too.
#ifdef dobogusinclude
#include <spi4teensy3.h>
#endif
#include <SPI.h>

#include <SoftwareSerial.h>
#include <Kangaroo.h>
#include <ros.h>
#include <geometry_msgs/Twist.h>
#include <std_msgs/String.h>
// Arduino TX (pin 11) goes to Kangaroo S1
// Arduino RX (pin 10) goes to Kangaroo S2
// Arduino GND         goes to Kangaroo 0V
// Arduino 5V          goes to Kangaroo 5V (OPTIONAL, if you want Kangaroo to power the Arduino)
#define TX_PIN 14
#define RX_PIN 15
// Independent mode channels on Kangaroo are, by default, '1' and '2'.
ros::NodeHandle nh;

geometry_msgs::Twist msg;
//ros::Publisher pub("pub", &msg);
double x,z;
void commandFromKeyboard( const geometry_msgs::Twist &action) {
  msg = action;
  x = msg.linear.x;
  z = msg.angular.z;
}

ros::Subscriber<geometry_msgs::Twist> sub("turtle1/cmd_vel", commandFromKeyboard);

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
bool change = true;
int lastusbinit = 0;
void setup() {
  // put your setup code here, to run once:
  nh.initNode();
  nh.subscribe(sub);
 // nh.advertise(pub);
  Serial3.begin(19200);
  Serial.begin(115200);
#if !defined(__MIPSEL__)
  while (!Serial); // Wait for serial port to connect - used on Leonardo, Teensy and other boards with built-in USB CDC serial connection
#endif
  K1.start();
  K1.home().wait();
  K2.start();
  K2.home().wait();
}

void loop() {
  // put your main code here, to run repeatedly:
  spd1 = (double)(K1.getMax().value() - K1.getMin().value()) / 40 ;
  spd2 = (double)(K2.getMax().value() - K2.getMin().value()) / 40 ;
  if (Usb.Init() == -1)
  { if (lastusbinit != -1 )
    {
      Serial.println("Changed to ROS: ");
      lastusbinit = -1;
    }
    ros_motion();
  }
  else
  { if (lastusbinit == -1)
    {
      Serial.println("Changed to PS3: ");
      lastusbinit = 0;
    }
    ps3_motion();
  }

}
void ps3_motion()
{
  Usb.Task();
  if (PS3.PS3Connected || PS3.PS3NavigationConnected)
  {
    lx = (double)PS3.getAnalogHat(LeftHatX);
    ry = (double)PS3.getAnalogHat(RightHatY);
    ry = 128 - ry;
  }
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
}
void kangaroo_speed_direction()
{ double rotfac;
  double sign = 1;
  if (ry <= 0)
    sign = -1;
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
  ry /= 128;
  double fac = 0;
  spd1 *= sign * (ry * ry);
  spd2 *= sign * (ry * ry);

  lx -= 128;
  fac = lx / 128;
  if ( fac >= 0)
  { spd1 = (1 + fac * fac) / 2 * spd1 + spd1m ;
    spd2 = (1 - fac * fac) / 2 * spd2 + spd2m ;
  }
  else if (fac < 0)
  { spd1 = (1 - fac * fac) / 2 * spd1 + spd1m ;
    spd2 = (1 + fac * fac) / 2 * spd2 + spd2m ;
  }
  K1.s(spd1).wait();
  K2.s(spd2).wait();

}
void ros_motion()
{ nh.spinOnce();
  spd1m = 0; spd2m = 0;
  double spd1r = 0 , spd2r = 0;
  if (x > 0)
  {
    spd1m = spd1 / 4;
    spd2m = spd2 / 4;
    //delay(50);
  }
  else if (x < 0)
  {
    spd1m = -spd1 / 4;
    spd2m = -spd2 / 4;
    //delay(50);
  }
  else if (z > 0)
  {
    spd1r = spd1 / 4;
    spd2r = -spd2 / 4;
    //delay(50);
  }
  else if (z <  0)
  {
    spd1r = -spd1 / 4;
    spd2r = spd2 / 4;
    //delay(50);
  }
  K1.s(spd1m + spd1r).wait();
  K2.s(spd2m + spd2r).wait();
  //pub.publish(&msg);
  
  delay(1);
}
