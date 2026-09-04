#include <LiquidCrystal.h>

// LCD: RS, E, D4, D5, D6, D7
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);


// ===============================
// REVERSE CLOCK
// ===============================

int hours = 12;
int minutes = 0;
int seconds = 0;


// ===============================
// REVERSE DATE
// ===============================

int day = 4;
int month = 9;
int year = 2026;


// ===============================
// SPEED CONTROL
// ===============================

int speedIndex = 0;

int speeds[] = {
  1,
  2,
  5,
  10,
  60
};


// ===============================
// SPEED GLITCH
// ===============================

bool speedGlitch = false;
int glitchSpeed = 1;

unsigned long glitchStart = 0;
unsigned long nextGlitch = 0;


// ===============================
// TIME TRAVEL
// ===============================

int timeTravelCount = 0;


// ===============================
// REVERSE ALARM
// ===============================

int alarmHour = 11;
int alarmMinute = 55;
int alarmSecond = 0;

bool alarmEnabled = true;
bool alarmTriggered = false;


// ===============================
// TIMING
// ===============================

unsigned long previousMillis = 0;


// ===============================
// BUTTON PINS
// ===============================

#define ALARM_BUTTON 6
#define SPEED_UP 7
#define SPEED_DOWN 8
#define TIME_TRAVEL 9


// ===============================
// LED PINS
// ===============================

#define GREEN_LED 10
#define YELLOW_LED A0
#define RED_LED A1


// ===============================
// BUZZER
// ===============================

#define BUZZER 13



// =====================================================
// DAYS IN MONTH
// =====================================================

int daysInMonth(int m, int y) {

  if (m == 2) {

    if ((y % 400 == 0) || ((y % 4 == 0) && (y % 100 != 0))) {
      return 29;
    }

    return 28;
  }

  if (m == 4 || m == 6 || m == 9 || m == 11) {
    return 30;
  }

  return 31;
}



// =====================================================
// SUBTRACT ONE DAY
// =====================================================

void reverseOneDay() {

  day--;

  if (day < 1) {

    month--;

    if (month < 1) {
      month = 12;
      year--;
    }

    day = daysInMonth(month, year);
  }
}



// =====================================================
// SUBTRACT ONE SECOND
// =====================================================

void reverseOneSecond() {

  seconds--;

  if (seconds < 0) {

    seconds = 59;
    minutes--;

    if (minutes < 0) {

      minutes = 59;
      hours--;

      if (hours < 0) {

        hours = 23;

        // Midnight reached
        // Reverse the date by one day
        reverseOneDay();
      }
    }
  }
}



// =====================================================
// SUBTRACT MANY SECONDS
// =====================================================

void reverseSeconds(int amount) {

  for (int i = 0; i < amount; i++) {
    reverseOneSecond();
  }
}



// =====================================================
// DISPLAY CLOCK + DATE
// =====================================================

void displayClock() {

  lcd.clear();

  // DATE
  lcd.setCursor(0, 0);

  if (day < 10) lcd.print("0");
  lcd.print(day);

  lcd.print("/");

  if (month < 10) lcd.print("0");
  lcd.print(month);

  lcd.print("/");

  lcd.print(year);


  // TIME
  lcd.setCursor(0, 1);

  if (hours < 10) lcd.print("0");
  lcd.print(hours);

  lcd.print(":");

  if (minutes < 10) lcd.print("0");
  lcd.print(minutes);

  lcd.print(":");

  if (seconds < 10) lcd.print("0");
  lcd.print(seconds);

  lcd.print(" ");

  lcd.print(speeds[speedIndex]);

  lcd.print("X");
}



// =====================================================
// SETUP
// =====================================================

void setup() {

  lcd.begin(16, 2);


  // Buttons
  pinMode(ALARM_BUTTON, INPUT_PULLUP);
  pinMode(SPEED_UP, INPUT_PULLUP);
  pinMode(SPEED_DOWN, INPUT_PULLUP);
  pinMode(TIME_TRAVEL, INPUT_PULLUP);


  // LEDs
  pinMode(GREEN_LED, OUTPUT);
  pinMode(YELLOW_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);


  // Buzzer
  pinMode(BUZZER, OUTPUT);


  // Normal state
  digitalWrite(GREEN_LED, HIGH);
  digitalWrite(YELLOW_LED, LOW);
  digitalWrite(RED_LED, LOW);
  digitalWrite(BUZZER, LOW);


  // Random seed
  randomSeed(analogRead(A5));


  // First speed glitch after 10 seconds
  nextGlitch = millis() + 10000;


  // Startup screen
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("REVERSE CLOCK");

  lcd.setCursor(0, 1);
  lcd.print("04/09/2026 1X");

  delay(2000);


  displayClock();

  previousMillis = millis();
}



// =====================================================
// LOOP
// =====================================================

void loop() {


  // ===================================================
  // ALARM BUTTON
  // ===================================================

  if (digitalRead(ALARM_BUTTON) == LOW) {

    alarmEnabled = !alarmEnabled;

    lcd.clear();

    lcd.setCursor(0, 0);

    if (alarmEnabled) {
      lcd.print("ALARM: ON");
    } else {
      lcd.print("ALARM: OFF");
    }

    lcd.setCursor(0, 1);
    lcd.print("11:55:00");


    tone(BUZZER, 800);
    delay(150);
    noTone(BUZZER);

    delay(800);

    displayClock();

    delay(300);
  }



  // ===================================================
  // SPEED UP
  // ===================================================

  if (digitalRead(SPEED_UP) == LOW) {

    speedIndex++;

    if (speedIndex > 4) {
      speedIndex = 4;
    }


    lcd.clear();

    lcd.setCursor(0, 0);
    lcd.print("SPEED:");

    lcd.print(speeds[speedIndex]);

    lcd.print("X");


    lcd.setCursor(0, 1);
    lcd.print("TIME GOES BACK");


    tone(BUZZER, 1000);
    delay(100);
    noTone(BUZZER);

    delay(700);

    displayClock();

    delay(300);
  }



  // ===================================================
  // SPEED DOWN
  // ===================================================

  if (digitalRead(SPEED_DOWN) == LOW) {

    speedIndex--;

    if (speedIndex < 0) {
      speedIndex = 0;
    }


    lcd.clear();

    lcd.setCursor(0, 0);
    lcd.print("SPEED:");

    lcd.print(speeds[speedIndex]);

    lcd.print("X");


    lcd.setCursor(0, 1);
    lcd.print("WHY SO SLOW?");


    tone(BUZZER, 500);
    delay(100);
    noTone(BUZZER);

    delay(700);

    displayClock();

    delay(300);
  }



  // ===================================================
  // TIME TRAVEL
  // ===================================================

  if (digitalRead(TIME_TRAVEL) == LOW) {

    timeTravelCount++;


    // Go back ONE DAY
    reverseOneDay();


    // Yellow LED
    digitalWrite(YELLOW_LED, HIGH);


    lcd.clear();

    lcd.setCursor(0, 0);
    lcd.print("TIME TRAVEL!");

    lcd.setCursor(0, 1);

    if (day < 10) lcd.print("0");
    lcd.print(day);

    lcd.print("/");

    if (month < 10) lcd.print("0");
    lcd.print(month);

    lcd.print("/");

    lcd.print(year);


    tone(BUZZER, 1200);
    delay(250);
    noTone(BUZZER);


    delay(700);


    digitalWrite(YELLOW_LED, LOW);


    // =================================================
    // TIME PARADOX
    // =================================================

    if (timeTravelCount >= 5) {

      lcd.clear();

      lcd.setCursor(0, 0);
      lcd.print("TIME PARADOX!");

      lcd.setCursor(0, 1);
      lcd.print("REALITY ERROR");


      digitalWrite(RED_LED, HIGH);


      tone(BUZZER, 700);

      delay(1500);

      noTone(BUZZER);

      delay(300);


      digitalWrite(RED_LED, LOW);


      timeTravelCount = 0;
    }


    displayClock();

    delay(400);
  }



  // ===================================================
  // RANDOM SPEED GLITCH
  // ===================================================

  if (!speedGlitch && millis() >= nextGlitch) {

    speedGlitch = true;

    glitchStart = millis();


    // Randomly choose 10X or 60X
    if (random(0, 2) == 0) {
      glitchSpeed = 10;
    } else {
      glitchSpeed = 60;
    }


    lcd.clear();

    lcd.setCursor(0, 0);
    lcd.print("SPEED GLITCH!");

    lcd.setCursor(0, 1);
    lcd.print("SYSTEM: ");

    lcd.print(glitchSpeed);

    lcd.print("X");


    digitalWrite(RED_LED, HIGH);


    tone(BUZZER, 400);
    delay(250);
    noTone(BUZZER);
  }



  // ===================================================
  // END SPEED GLITCH
  // ===================================================

  if (speedGlitch && millis() - glitchStart >= 3000) {

    speedGlitch = false;


    digitalWrite(RED_LED, LOW);


    // Next glitch in 10–20 seconds
    nextGlitch = millis() + random(10000, 20001);


    displayClock();
  }



  // ===================================================
  // REVERSE CLOCK
  // ===================================================

  if (millis() - previousMillis >= 1000) {

    previousMillis = millis();


    int amountToReverse;


    if (speedGlitch) {
      amountToReverse = glitchSpeed;
    } else {
      amountToReverse = speeds[speedIndex];
    }


    // Reverse the time
    reverseSeconds(amountToReverse);


    // =================================================
    // REVERSE ALARM
    // =================================================

    if (alarmEnabled && !alarmTriggered) {

      if (hours == alarmHour &&
          minutes == alarmMinute &&
          seconds == alarmSecond) {


        alarmTriggered = true;


        lcd.clear();

        lcd.setCursor(0, 0);
        lcd.print("REVERSE ALARM!");

        lcd.setCursor(0, 1);
        lcd.print("WAKE UP???");


        digitalWrite(RED_LED, HIGH);


        tone(BUZZER, 1000);
        delay(700);

        noTone(BUZZER);

        delay(500);


        digitalWrite(RED_LED, LOW);


        lcd.clear();

        lcd.setCursor(0, 0);
        lcd.print("ALARM PASSED!");

        lcd.setCursor(0, 1);
        lcd.print("TIME GOES BACK");


        delay(1200);


        displayClock();
      }
    }


    // =================================================
    // NORMAL DISPLAY
    // =================================================

    if (!speedGlitch && !alarmTriggered) {
      displayClock();
    }
  }
}
