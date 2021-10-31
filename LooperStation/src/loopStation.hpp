
#include "Audio.h"
#include <Arduino.h>
#include <Bounce.h>
#include <SD.h>
#include <SPI.h>
#include <SerialFlash.h>
#include <Wire.h>

typedef enum recordState { RECORDING, FREE };
typedef enum playState { START, STOP };

#define SD_CS 10

void updateFilename();

class loopStation {
public:
  void init();
  void playChannel(int id);
  void recordChannel();
  void recordChannelStop(int id);
  void stopChannel(int id);

  void serviceRoutine();

  playState channelPlayState[4] = {STOP, STOP, STOP, STOP};
  // int channelRecordState[4];
  recordState recState = FREE;

  // buffer
};
