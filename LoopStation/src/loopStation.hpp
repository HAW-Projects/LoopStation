
#include "Audio.h"
#include "play_sd_buffer.h"
#include <Arduino.h>
#include <Bounce.h>
#include <SD.h>
#include <SPI.h>
#include <SerialFlash.h>
#include <Wire.h>

enum recordState { RECORDING, FREE };
enum playState { START, STOP };

#define BUFFERSIZE 512

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

  String playFilenames[4];
  FsFile playFiles[4];
  FsFile recordFile;

  // buffer
  byte readBuffer[4][2][BUFFERSIZE]; // play Buffer 4x Buffersize
  byte playBufferIndex[4];

  uint32_t file_size[4];
  volatile uint32_t file_offset[4];
};
