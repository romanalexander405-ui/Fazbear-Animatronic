#include <Servo.h>
#include <ezButton.h>

/************************************************************
 *  SECTION 1 — Photocell + LEDs + Servo Oscillation
 ************************************************************/

Servo myservo1;

// ---- Photocell & Threshold ----
int photocellPin = A0;
int sensorValue1 = 0;
int threshold1 = 200;

// ---- Servo + LEDs ----
int servoPin1 = 13;
int led1A = 11;
int led2A = 12;

bool oscillate1 = false;
int pos1 = 0;
bool wasDark = false;   // track previous state

void runPhotocellSystem() {
  sensorValue1 = analogRead(photocellPin);
  Serial.print("Photocell: ");
  Serial.println(sensorValue1);

  bool isDark = (sensorValue1 < threshold1);

  // Just turned DARK (light -> dark)
  if (isDark && !wasDark) {
    digitalWrite(led1A, HIGH);
    digitalWrite(led2A, HIGH);
    oscillate1 = true;

    Serial.println("PLAY1");   // play YouTube #1 once
    wasDark = true;
  }
  // Still dark
  else if (isDark && wasDark) {
    digitalWrite(led1A, HIGH);
    digitalWrite(led2A, HIGH);
    oscillate1 = true;
  }
  // Just turned BRIGHT (dark -> light)
  else if (!isDark && wasDark) {
    digitalWrite(led1A, LOW);
    digitalWrite(led2A, LOW);
    oscillate1 = false;
    myservo1.write(0);   // reset servo
    wasDark = false;
  }

  // Oscillate ONLY when dark
  if (oscillate1) {
    // 0 → 180
    for (pos1 = 0; pos1 <= 180 && oscillate1; pos1++) {
      myservo1.write(pos1);
      delay(10);

      // stop if it becomes bright
      if (analogRead(photocellPin) >= threshold1) {
        oscillate1 = false;
        break;      // wasDark stays true — cleanup happens next loop
      }
    }
    // 180 → 0
    for (pos1 = 180; pos1 >= 0 && oscillate1; pos1--) {
      myservo1.write(pos1);
      delay(10);

      if (analogRead(photocellPin) >= threshold1) {
        oscillate1 = false;
        break;      // LEDs will turn off next runPhotocellSystem() call
      }
    }
  }
}

/************************************************************
 *  SECTION 2 — Joystick Button → Toggle Servo Oscillation
 ************************************************************/

Servo myservo2;
#define SW_PIN 10   // joystick button

ezButton button(SW_PIN);

bool oscillate2 = false;
int pos2 = 0;

void runJoystickOscillation() {
  button.loop();

  // Toggle ON/OFF when pressed
  if (button.isPressed()) {
    oscillate2 = !oscillate2;
    Serial.print("Joystick toggle = ");
    Serial.println(oscillate2);
    Serial.println("PLAY2");   // play YouTube #2
  }

  // Sweep when ON
  if (oscillate2) {
    for (pos2 = 0; pos2 <= 180 && oscillate2; pos2++) {
      myservo2.write(pos2);
      delay(10);
      button.loop();
      if (button.isPressed()) { oscillate2 = false; break; }
    }

    for (pos2 = 180; pos2 >= 0 && oscillate2; pos2--) {
      myservo2.write(pos2);
      delay(10);
      button.loop();
      if (button.isPressed()) { oscillate2 = false; break; }
    }
  }
}

/************************************************************
 *  SETUP + LOOP
 ************************************************************/

void setup() {
  Serial.begin(9600);

  // Section 1
  myservo1.attach(servoPin1);
  myservo1.write(0);
  pinMode(led1A, OUTPUT);
  pinMode(led2A, OUTPUT);

  // Section 2
  button.setDebounceTime(50);
  myservo2.attach(9);   // joystick servo pin
  myservo2.write(0);
}

void loop() {
  runPhotocellSystem();
  runJoystickOscillation();
}
