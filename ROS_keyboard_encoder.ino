#include <Kangaroo.h>

#include <ros.h>
#include <geometry_msgs/Twist.h>
#include <std_msgs/String.h>
#include <geometry_msgs/Point.h>

ros::NodeHandle nh;

KangarooSerial  K(Serial3);
KangarooChannel K1(K, '2');
KangarooChannel K2(K, '1');

void commandFromKeyboard( const geometry_msgs::Twist &msg){
  long minimum = K1.getMin().value();
  long maximum = K1.getMax().value();
  long speed   = (maximum - minimum) / 100;
  if (msg.linear.x == 2)
  {
    K1.s(speed);
    K2.s(speed);
    delay(500);
  }
  else if (msg.linear.x == -2)
  {
    K1.s(-speed);
    K2.s(-speed);
    delay(500);
  }
  else if (msg.angular.z == 2)
  {
    K1.s(speed);
    K2.s(-speed);
    delay(500);
  }
  else if (msg.angular.z == -2)
  {
    K1.s(-speed);
    K2.s(speed);
    delay(500);
  }
  K1.s(0);
  K2.s(0);
}

ros::Subscriber<geometry_msgs::Twist> sub("turtle1/cmd_vel", commandFromKeyboard);

geometry_msgs::Point v;
ros::Publisher pub("speeds", &v);

void setup() {
  // put your setup code here, to run once:
  nh.initNode();
  nh.subscribe(sub);  
  nh.advertise(pub);
  Serial3.begin(19200);  
  K1.start();
  K1.home().wait();
  K2.start();
  K2.home().wait();
}

void loop() {
  // put your main code here, to run repeatedly:
  v.x = K1.getS().value();
  v.y = K2.getS().value();
  v.z = 0;
  pub.publish(&v);
  nh.spinOnce();
  delay(5);
}
