#include <Servo.h>
#include <PS2X_lib.h>

PS2X ps2x;
int error = 0;
byte type = 0;
bool controllerConnected = false;

Servo myServo;
Servo DRServo;

int servoPin = 50;
int DRSPin = 35;

int in1 = 7;
int in2 = 6;
int enA = 3;

int in3 = 5;
int in4 = 4;
int enB = 2;

int buzzer = 8;

int red_pin = 41;
int blue_pin = 40;
int green_pin = 42;

bool isButtonPressed = false;
bool previousButtonState = false;
bool DRS = false;

unsigned long casovacZaciatok = 0;
unsigned long casovacKoniec = 0;
unsigned long uplynulyCas = 0;

int energia = 0;
int maxEnergia = 100;

void setup() {
  Serial.begin(115200);

  pinMode(buzzer, OUTPUT);
  pinMode(enA, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(enB, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);
  pinMode(red_pin, OUTPUT);
  pinMode(blue_pin, OUTPUT);
  pinMode(green_pin, OUTPUT);

  while (!controllerConnected) {
    error = ps2x.config_gamepad(13, 11, 10, 12, true, true);
    if (error == 0) {
      Serial.println("Ovládač nájdený, konfigurácia úspešná");
      controllerConnected = true;
    } else {
      Serial.println("Žiadny ovládač nenájdený, skontrolujte pripojenie, pozrite readme.txt pre aktiváciu debugu.");
      digitalWrite(red_pin, HIGH);
      digitalWrite(green_pin, LOW);
      digitalWrite(blue_pin, LOW);
      delay(1000);
    }
  }

  type = ps2x.readType();
  switch (type) {
    case 0:
      Serial.println("Neznámy typ ovládača");
      break;
    case 1:
      Serial.println("DualShock/PS2 Ovládač nájdený");
      break;
  }

  myServo.attach(servoPin);
  DRServo.attach(DRSPin);
}

void loop() {
  ps2x.read_gamepad();
  int servoPositionX = map(ps2x.Analog(PSS_LX), 0, 255, 180, 0);
  myServo.write(servoPositionX);
  delay(15);

  int motorSpeed = map(ps2x.Analog(PSS_RY), 0, 255, -200, 200);

  if (!isButtonPressed && energia > 0) {
    motorSpeed = map(motorSpeed, -200, 0, -200, 0); 
    motorSpeed = map(motorSpeed, 0, 200, 0, 200);   
  } else {
    motorSpeed = map(motorSpeed, -200, 0, -150, 0);
    motorSpeed = map(motorSpeed, 0, 200, 0, 150);
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
      if (energia < maxEnergia) {
        energia++; // Pridáme energiu
        Serial.println("Nabíjanie...");
        Serial.println(energia);
      }
  }

  if (ps2x.Button(PSB_GREEN) && !previousButtonState) {
    isButtonPressed = !isButtonPressed;
    previousButtonState = true;
    delay(100);

    if (ps2x.Button(PSB_GREEN)) {
      Serial.println("Tlačidlo je ZAPNUTÉ");
      digitalWrite(red_pin, LOW);
      digitalWrite(green_pin, LOW);
      digitalWrite(blue_pin, HIGH);
      casovacZaciatok = millis();
      casovacKoniec = casovacZaciatok + 10000; // 10 sekúnd
      
    } else {
      Serial.println("Tlačidlo je VYPNUTÉ");
      digitalWrite(red_pin, LOW);
      digitalWrite(green_pin, LOW);
      digitalWrite(blue_pin, HIGH);
      uplynulyCas = (millis() - casovacZaciatok) / 1000;
      casovacKoniec = 0;
      Serial.print("Uplynulý čas (s): ");
      Serial.println(uplynulyCas);
    }
  }

  if (!ps2x.Button(PSB_GREEN)) {
    previousButtonState = false;
  }

  if (ps2x.ButtonPressed(PSB_L1)){
    if(DRS == false){
      DRServo.write(90);
      DRS = true;
    } else {
      DRServo.write(0);
      DRS = false;
    }
    delay(500);
  }

  Serial.println(motorSpeed);


  if (isButtonPressed) {
    if (energia < maxEnergia) {
      Serial.println("Nabíjanie...");
    } else {
      Serial.println("Energia je plná!");
      digitalWrite(red_pin, LOW);
      digitalWrite(green_pin, HIGH);
      digitalWrite(blue_pin, LOW);
    }
  } else {
    if (energia > 0) {
      energia--;
      Serial.println("Vybíjanie...");
    } else {
      Serial.println("Energia je úplne vybitá!");
      digitalWrite(red_pin, HIGH);
      digitalWrite(green_pin, LOW);
      digitalWrite(blue_pin, LOW);
    }
  }

  int percentoEnergie = (energia * 100) / maxEnergia;
  Serial.print("Percento energie: ");
  Serial.print(percentoEnergie);
  Serial.println("%");

  unsigned long teraz = millis();
  if (casovacKoniec > 0 && !isButtonPressed) {
    if (teraz < casovacKoniec) {
      unsigned long zostavajuciCas = (casovacKoniec - teraz) / 1000;
      Serial.print("Zostávajúci čas (s): ");
      Serial.println(zostavajuciCas);
    } else {
      Serial.println("Čas vypršal!");
      casovacKoniec = 0;
    }
  }
}
