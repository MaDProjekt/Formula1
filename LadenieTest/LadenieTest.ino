#include <PS2X_lib.h>  //for v1.6
#include <Servo.h>
#include <SoftwareSerial.h>

SoftwareSerial mySerial(8, 9);// RX,TX

/******************************************************************
 * set pins connected to PS2 controller:
 *   - 1e column: original 
 *   - 2e colmun: Stef?
 * replace pin numbers by the ones you use
 ******************************************************************/
#define PS2_DAT        13  //14    
#define PS2_CMD        11  //15
#define PS2_SEL        10  //16
#define PS2_CLK        12  //17

/******************************************************************
 * select modes of PS2 controller:
 *   - pressures = analog reading of push-butttons 
 *   - rumble    = motor rumbling
 * uncomment 1 of the lines for each mode selection
 ******************************************************************/
#define pressures   true
// #define pressures   false
#define rumble      true
// #define rumble      false

PS2X ps2x; // create PS2 Controller Class

//right now, the library does NOT support hot pluggable controllers, meaning 
//you must always either restart your Arduino after you connect the controller, 
//or call config_gamepad(pins) again after connecting the controller.

int in1 = 7;
int in2 = 6;
int enA = 3;

int in3 = 5;
int in4 = 4;
int enB = 2;

int motorSpeed;

int red_pin = 41;
int blue_pin = 40;
int green_pin = 42;

Servo ZatServo;
Servo DRServo;

String DRS = "OFF";

bool stlacene = false;

int servoPin = 31;
int DRSPin = 35;
int LX;
int mapLX;

int error = 0;
byte type = 0;
byte vibrate = 0;

int energia = 0;
int maxEnergia = 100;
int percentoEnergie;

String Brzda;

unsigned long tk;
int mtk;
String ss = "";

void setup(){
 
  Serial.begin(57600);
  mySerial.begin(57600);
  
  pinMode(enA, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(enB, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);

  delay(300);  //added delay to give wireless ps2 module some time to startup, before configuring it
   
  ZatServo.attach(servoPin);
  DRServo.attach(DRSPin);

  Serial.println("Start..");
  digitalWrite(red_pin, HIGH);
  digitalWrite(green_pin, HIGH);
  digitalWrite(blue_pin, HIGH);
  delay(1000);
  ZatServo.write(80);
  delay(1000);
  ZatServo.write(140);
  delay(1000);
  ZatServo.write(80);
  Serial.println("Koniec");

  //CHANGES for v1.6 HERE!!! **************PAY ATTENTION*************
  
  //setup pins and settings: GamePad(clock, command, attention, data, Pressures?, Rumble?) check for error
  error = ps2x.config_gamepad(PS2_CLK, PS2_CMD, PS2_SEL, PS2_DAT, pressures, rumble);
  
  if(error == 0){
    Serial.print("Found Controller, configured successful ");
    digitalWrite(red_pin, LOW);
    digitalWrite(green_pin, HIGH);
    digitalWrite(blue_pin, LOW);
    Serial.print("pressures = ");
	if (pressures)
	  Serial.println("true ");
	else
	  Serial.println("false");
	Serial.print("rumble = ");
	if (rumble)
	  Serial.println("true)");
	else
	  Serial.println("false");
  }  
  else if(error == 1)
    Serial.println("No controller found, check wiring, see readme.txt to enable debug. visit www.billporter.info for troubleshooting tips");
   
  else if(error == 2)
    Serial.println("Controller found but not accepting commands. see readme.txt to enable debug. Visit www.billporter.info for troubleshooting tips");

  else if(error == 3)
    Serial.println("Controller refusing to enter Pressures mode, may not support it. ");
  
//  Serial.print(ps2x.Analog(1), HEX);
  
  type = ps2x.readType(); 
  switch(type) {
    case 0:
      Serial.print("Unknown Controller type found ");
      break;
    case 1:
      Serial.print("DualShock Controller found ");
      break;
   }
}

void loop() {
  /* You must Read Gamepad to get new values and set vibration values
     ps2x.read_gamepad(small motor on/off, larger motor strenght from 0-255)
     if you don't enable the rumble, use ps2x.read_gamepad(); with no values
     You should call this at least once a second
   */  
  if(error == 1) //skip loop if no controller found
    return; 
  
  if(type == 1) { //DualShock Controller
    ps2x.read_gamepad(false, motorSpeed); //read controller and set large motor to spin at 'vibrate' speed
    
    if(ps2x.Button(PSB_START))         //will be TRUE as long as button is pressed
      Serial.println("Start is being held");
    if(ps2x.Button(PSB_SELECT))
      Serial.println("Select is being held");      


    if (ps2x.NewButtonState()) {        //will be TRUE if any button changes state (on to off, or off to on)
      if(ps2x.Button(PSB_L3))
        Serial.println("L3 pressed");
      if(ps2x.Button(PSB_R3))
        Serial.println("R3 pressed");
      if(ps2x.Button(PSB_L2))
        Serial.println("L2 pressed");
      if(ps2x.Button(PSB_R2))
        Serial.println("R2 pressed");
      if(ps2x.Button(PSB_TRIANGLE))
        Serial.println("Triangle pressed");        
    }

    if(ps2x.ButtonPressed(PSB_CIRCLE))               //will be TRUE if button was JUST pressed
      Serial.println("Circle just pressed");
    if(ps2x.NewButtonState(PSB_CROSS))               //will be TRUE if button was JUST pressed OR released
      Serial.println("X just changed");
    if(ps2x.ButtonReleased(PSB_SQUARE))              //will be TRUE if button was JUST released
      Serial.println("Square just released");     

    // if(ps2x.Button(PSB_L1) || ps2x.Button(PSB_R1)) { //print stick values if either is TRUE
    if(ps2x.ButtonPressed(PSB_START)){
      stlacene = !stlacene;
      Serial.println(stlacene);
    }

    if (stlacene){
      // motorSpeed = map(ps2x.Analog(PSS_RY), 0, 255, 255, -255);

      //motorSpeed = map(ps2x.Analog(PSS_RY), 0, 255, 200, -200);

      if (ps2x.Button(PSB_R1) && energia > 0) {
        motorSpeed = map(ps2x.Analog(PSS_RY), 0, 255, 255, -255);   
      } else {
        //motorSpeed = map(motorSpeed, -200, 0, -200, 0);
        motorSpeed = map(ps2x.Analog(PSS_RY), 0, 255, 200, -200);
      }
    

      if (motorSpeed < 0) {
        analogWrite(enA, -motorSpeed);
        digitalWrite(in1, LOW);
        digitalWrite(in2, HIGH);
        analogWrite(enB, -motorSpeed);
        digitalWrite(in3, LOW);
        digitalWrite(in4, HIGH);
      } else if (motorSpeed > 0) {
        analogWrite(enA, motorSpeed);
        digitalWrite(in1, HIGH);
        digitalWrite(in2, LOW);
        analogWrite(enB, motorSpeed);
        digitalWrite(in3, HIGH);
        digitalWrite(in4, LOW);
      } else {
        analogWrite(enA, 0);
        analogWrite(enB, 0);
      }

      if(ps2x.Button(PSB_L2)){
        analogWrite(enA, -180);
        analogWrite(enB, -180);
      }


        if (ps2x.Button(PSB_L2)) {
          Serial.println("Drzis brzdu");
          Brzda = "300";
        if (energia < maxEnergia) {
            energia++;
            Serial.println("Nabíjanie...");
            Serial.println(energia);
            digitalWrite(red_pin, LOW);
            digitalWrite(green_pin, LOW);
            digitalWrite(blue_pin, HIGH);        
          } else {
            Serial.println("Energia je plná!");
            digitalWrite(red_pin, LOW);
            digitalWrite(green_pin, HIGH);
            digitalWrite(blue_pin, LOW);
          }
        }else{
          Brzda = "0";
        }

         if(ps2x.Button(PSB_R1)){
          
          if (energia > 0) {
            energia--;
          Serial.println("Vybíjanie...");
          digitalWrite(red_pin, LOW);
          digitalWrite(green_pin, LOW);
          digitalWrite(blue_pin, HIGH);
        } else {
          Serial.println("Energia je úplne vybitá!");
          digitalWrite(red_pin, HIGH);
          digitalWrite(green_pin, LOW);
          digitalWrite(blue_pin, LOW);
        }
      }

      percentoEnergie = (energia * 100) / maxEnergia;
      Serial.print("Percento energie: ");
      Serial.print(percentoEnergie);
      Serial.println("%");


      if (ps2x.ButtonPressed(PSB_L1)){
          if(DRS == "OFF"){
            DRServo.write(10);
            DRS = "ON";
          } else {
            DRServo.write(2);
            DRS = "OFF";
          }
          delay(500);
      }




      //Serial.print("Stick Values:");
      //Serial.print(ps2x.Analog(PSS_LY), DEC); //Left stick, Y axis. Other options: LX, RY, RX  
      //Serial.print(",");
      LX = ps2x.Analog(PSS_LX);
      //Serial.print(LX, DEC); 
      //Serial.print(",");
      //Serial.print(PSS_LY, DEC); 
      //Serial.print(",");
      //Serial.print(motorSpeed, DEC);
      mapLX = map(LX, 0, 255, 150, 70);
      //Serial.print(",");
      //Serial.print(mapLX, DEC);
      //Serial.print(",");
      //Serial.println(stlacene);
      Serial.println(Brzda);

      ZatServo.write(mapLX);
      
    }     


     Serial.print("Time: ");
  tk = (long) millis() / 1000;
  mtk = tk % 2;

  Serial.print(tk);
  Serial.println(" , " + String(mtk));

  if (mtk == 0){
    Serial.println("Posielam data!");
    ss = String(motorSpeed * 2) + ";" + String(DRS) + ";"+ String(percentoEnergie) + ";" + String(Brzda) + ";" + String(mapLX) + "|";
    mySerial.write(ss.c_str());
    Serial.println(ss);
  }
  }
  delay(50);  
}