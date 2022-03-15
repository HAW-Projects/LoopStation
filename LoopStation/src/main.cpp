#include "loopStation.hpp"
#include <Arduino.h>

#include <Bounce.h>

// Bounce objects to easily and reliably read the buttons
Bounce buttonRecord = Bounce(37, 8);
Bounce buttonStop = Bounce(38, 8); // 8 = 8 ms debounce time
Bounce buttonPlay = Bounce(39, 8);
Bounce buttonStartRecPlay = Bounce(36, 8);
Bounce buttonPlay2Ch = Bounce(35, 8);

loopStation station;
void updateButton();

void setup() {

  Serial.begin(9600);
  while (!Serial)
    ;
  Serial.println("Hello");

  pinMode(37, INPUT_PULLUP);
  pinMode(38, INPUT_PULLUP);
  pinMode(39, INPUT_PULLUP);
  pinMode(36, INPUT_PULLUP);
  pinMode(35, INPUT_PULLUP);

  station.init();

  Serial.println("Init Done!");
}

void loop() {
  station.serviceRoutine();
  updateButton();
  // led.update();
}

void updateButton() {

  buttonRecord.update();
  buttonStop.update();
  buttonPlay.update();

  // Respond to button presses
  if (buttonRecord.fallingEdge()) {
    Serial.println("Record Button Press");
    station.recordChannel();
  }
  if (buttonStop.fallingEdge()) {
    Serial.println("Stop Button Press");
    station.recordChannelStop(0);
  }
  if (buttonPlay.fallingEdge()) {
    Serial.println("Play Button Press");
    station.recordChannelStop(1);
  }

  if (buttonPlay2Ch.fallingEdge()) {
    Serial.println("PlayCH2 Button Press");
    station.playChannel(1);
  }
}