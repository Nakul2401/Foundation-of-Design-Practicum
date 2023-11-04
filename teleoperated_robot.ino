#include <ros.h>
#include <geometry_msgs/Twist.h>
#include <ESP8266WiFi.h>

#define EN_L D0
#define IN1_L D1
#define IN2_L D2
#define EN_R D5
#define IN1_R D3
#define IN2_R D4

const char *ssid =  "Dev patel";
const char *pass =  "devpatel08";
IPAddress server(192,168,9,6);

double w_r=0, w_l=0;
double wheel_rad = 0.041, wheel_sep = 0.250;  //wheel_rad is the wheel radius ,wheel_sep is

WiFiClient client;

class WiFiHardware {
public:
  WiFiHardware(){};

  void init() {
    // do your initialization here. this probably includes TCP server/client setup
    client.connect(server, 11411);
  }

  // read a byte from the serial port. -1 = failure
  int read() {
    // implement this method so that it reads a byte from the TCP connection and returns it
    //  you may return -1 is there is an error; for example if the TCP connection is not open
    return client.read();  //will return -1 when it will works
  }

  // write data to the connection to ROS
  void write(uint8_t *data, int length) {
    // implement this so that it takes the arguments and writes or prints them to the TCP connection
    for (int i = 0; i < length; i++)
      client.write(data[i]);
  }

  // returns milliseconds since start of program
  unsigned long time() {
    return millis();  // easy; did this one for you
  }
};

ros::NodeHandle_<WiFiHardware> nh;

//int lowSpeed = 200;
//int highSpeed = 50;
double speed_ang=0, speed_lin=0;

void messageCb( const geometry_msgs::Twist& msg){
  speed_ang = msg.angular.z;
  speed_lin = msg.linear.x;
  w_r = (speed_lin/wheel_rad) + ((speed_ang*wheel_sep)/(2.0*wheel_rad));
  w_l = (speed_lin/wheel_rad) - ((speed_ang*wheel_sep)/(2.0*wheel_rad));
}

ros::Subscriber<geometry_msgs::Twist> sub("cmd_vel", &messageCb );

void Motors_init();
void MotorL(int Pulse_Width1);
void MotorR(int Pulse_Width2);

void setup(){
   Serial.begin(115200);
   delay(100);
               
   Serial.println("Connecting to ");
   Serial.println(ssid); 
 
   WiFi.begin(ssid, pass); 
   while (WiFi.status() != WL_CONNECTED) 
   {
      delay(500);
      Serial.print(".");
   }
   Serial.println("");
   Serial.println("WiFi connected");

   pinMode(D6,OUTPUT);
   for(int i=0;i<3;i++){
    digitalWrite(D6,HIGH);
    delay(200);
    digitalWrite(D6,LOW);
    delay(200);
   }
   
   Motors_init();
   nh.initNode();
   nh.subscribe(sub);

}

void loop(){
 MotorL(w_l*10);
 MotorR(w_r*10);
 Serial.println("-----");
 if(w_l!=0 and w_r!=0){digitalWrite(D6,HIGH);}
 else{digitalWrite(D6,LOW);}
 nh.spinOnce();
}

void Motors_init(){
 pinMode(EN_L, OUTPUT);
 pinMode(EN_R, OUTPUT);
 pinMode(IN1_L, OUTPUT);
 pinMode(IN2_L, OUTPUT);
 pinMode(IN1_R, OUTPUT);
 pinMode(IN2_R, OUTPUT);
 digitalWrite(EN_L, LOW);
 digitalWrite(EN_R, LOW);
 digitalWrite(IN1_L, LOW);
 digitalWrite(IN2_L, LOW);
 digitalWrite(IN1_R, LOW);
 digitalWrite(IN2_R, LOW);
}
void MotorL(int Pulse_Width1){
 if (Pulse_Width1 > 0){
     analogWrite(EN_L, Pulse_Width1);
     digitalWrite(IN1_L, HIGH);
     digitalWrite(IN2_L, LOW);
     Serial.println("Forward LEFT");
     Serial.println(Pulse_Width1);
 }
 else if (Pulse_Width1 < 0){
     Pulse_Width1=abs(Pulse_Width1);
     analogWrite(EN_L, Pulse_Width1);
     digitalWrite(IN1_L, LOW);
     digitalWrite(IN2_L, HIGH);
     Serial.println("Backward LEFT");
     Serial.println(Pulse_Width1);
 }
 else{
     analogWrite(EN_L, Pulse_Width1);
     Serial.println("Stop LEFT");
     Serial.println(Pulse_Width1);
     digitalWrite(IN1_L, LOW);
     digitalWrite(IN2_L, LOW);
 }
}
void MotorR(int Pulse_Width2){
 if (Pulse_Width2 > 0){
     analogWrite(EN_R, Pulse_Width2);
     digitalWrite(IN1_R, LOW);
     digitalWrite(IN2_R, HIGH);
     Serial.println("Forward Right");
     Serial.println(Pulse_Width2);
 }
 else if (Pulse_Width2 < 0){
     Pulse_Width2=abs(Pulse_Width2);
     analogWrite(EN_R, Pulse_Width2);
     digitalWrite(IN1_R, HIGH);
     digitalWrite(IN2_R, LOW);
     Serial.println("Backward Right");
     Serial.println(Pulse_Width2);
 }
 else{
     analogWrite(EN_R, Pulse_Width2);
     digitalWrite(IN1_R, LOW);
     digitalWrite(IN2_R, LOW);
     Serial.println("Stop Right");
     Serial.println(Pulse_Width2);
 }
}
