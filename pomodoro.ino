#include <Arduino.h>
#include <TM1637Display.h>
#include <virtuabotixRTC.h>
virtuabotixRTC myRTC(6, 7, 8);

// digital pins for TM1637
#define CLK 4
#define DIO 5
TM1637Display display(CLK, DIO);

long PoTime = 1500; // 1500 seconds = 25 minutes
int Past = -1;
bool isRunning = false; // Status: false = stop, true = running
int TogglePIN = 2; // Uses for start and stop
int setPomoPIN = 3;
int setBreakPIN = 10;
int buzzerPIN = 9;

bool lastToggleState = HIGH; // for the Toggle button

void setup()
{
  display.setBrightness(7);
  pinMode(buzzerPIN, OUTPUT);
  pinMode(TogglePIN, INPUT_PULLUP );
  pinMode(setPomoPIN, INPUT_PULLUP);
  pinMode(setBreakPIN, INPUT_PULLUP);
  // Display 25:00 on the screen
  display.showNumberDecEx(2500, 0b01000000, true); // 0b01000000 to show the dots
}

void loop()
{
  int ToggleState = digitalRead(TogglePIN);
  int PomoState = digitalRead(setPomoPIN);
  int BreakState = digitalRead(setBreakPIN);

  //------------------BUTTONS-----------------//

  // Press once = Start , Press Twice = Stop
  if (ToggleState == LOW && lastToggleState == HIGH) {
    isRunning = !isRunning; // Switch between true and false.
    delay(100); // Debounce
  }
  lastToggleState = ToggleState; // Save the state for comparison next time.

  if (PomoState == LOW) {
    PoTime = 1500; // Reset to 25mins
    isRunning = false;
    display.showNumberDecEx(2500, 0b01000000, true);
  }

  if (BreakState == LOW) {
    PoTime = 900;          // 900 seconds = 15minutes
    isRunning = false;     // stop after reset
    display.showNumberDecEx(1500, 0b01000000, true);
  }

  //------------------COUNTDOWN-----------------//

  myRTC.updateTime();
  if (isRunning && (myRTC.seconds != Past)) {
    Past = myRTC.seconds;

    if (PoTime > 0) {
      PoTime--;
    } else if (PoTime == 0 && isRunning) {
      alertDone();
      isRunning = false;
    }

    //------------------DISPLAY-----------------//
    int minutes = PoTime / 60;
    int seconds = PoTime % 60;
    int finalTime = (minutes * 100) + seconds;
    display.showNumberDecEx(finalTime, 0b01000000, true);
  }
}

//------------------ALERT-----------------//

void alertDone() {
  for (int i = 0; i < 6; i++) { // Alert 6 times
    tone(buzzerPIN, 2500);     // 2500Hz
    delay(150);
    noTone(buzzerPIN);
    delay(100);
  }
}
