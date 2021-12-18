#include <LiquidCrystal_I2C.h>
#include <BigNumbers_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);
BigNumbers_I2C bigNum( & lcd);

const int buzzer = 13;
const int maxPeople = 5;
const int sensitivity = 5;
const int sensor1[] = {4,5};
const int sensor2[] = {6,7};

int sensor1Init;
int sensor2Init;
int timeoutCounter = 0;
String sequence = "";
int currentPeople = 0;

void setup() {
  Wire.begin();
  Serial.begin(9600);
  pinMode(buzzer, OUTPUT);
  lcd.begin(16, 2);
  lcd.backlight();
  bigNum.begin();
  lcd.setCursor(0, 0);
  lcd.print("OCCUPANCY");
  lcd.setCursor(0, 1);
  lcd.print("COUNTER");
  delay(500);
  sensor1Init = measureDistance(sensor1);
  sensor2Init = measureDistance(sensor2);
}

void loop() {
  int sensor1Val = measureDistance(sensor1);
  int sensor2Val = measureDistance(sensor2);

  if (sensor1Val < sensor1Init - 30 && sequence.charAt(0) != '1') {
    sequence += "1";
  } else if (sensor2Val < sensor2Init - 30 && sequence.charAt(0) != '2') {
    sequence += "2";
  }

  if (sequence.equals("12") && currentPeople < 6) {
    currentPeople++;
    sequence = "";
    delay(550);
  } else if (sequence.equals("21") && currentPeople > 0) {
    currentPeople--;
    sequence = "";
    delay(550);
  }

  if (sequence.length() > 2 || sequence.equals("11") || sequence.equals("22") || timeoutCounter > 200) {
    sequence = "";
  }

  if (sequence.length() == 1) { //
    timeoutCounter++;
  } else {
    timeoutCounter = 0;
  }

  bigNum.displayLargeNumber(currentPeople, 13, 0);

  if (currentPeople > maxPeople) {
    tone(buzzer, 1700);
  } else {
    noTone(buzzer);
  }
}

int measureDistance(const int a[]) {
  pinMode(a[1], OUTPUT);
  digitalWrite(a[1], LOW);
  delayMicroseconds(2);
  digitalWrite(a[1], HIGH);
  delayMicroseconds(10);
  digitalWrite(a[1], LOW);
  pinMode(a[0], INPUT);
  long duration = pulseIn(a[0], HIGH, 100000);
  return duration / 29 / 2;
}