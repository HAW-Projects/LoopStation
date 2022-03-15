#include "loopStation.hpp"

AudioInputI2S i2s2;       // xy=651,667
AudioAnalyzePeak peak1;   // xy=824,655
AudioRecordQueue queue1;  // xy=835,560
AudioPlayBuff playRaw[4]; // xy=716,596
AudioMixer4 mixer1;       // xy=946,601
AudioOutputI2S i2s1;      // xy=1169,670
AudioConnection qpatchCord7(i2s2, 0, queue1, 0);
AudioConnection patchCord8(i2s2, 0, peak1, 0);
AudioConnection patchCord2(playRaw[0], 0, mixer1, 0);
AudioConnection patchCord1(playRaw[1], 0, mixer1, 1);
AudioConnection patchCord3(playRaw[2], 0, mixer1, 2);
AudioConnection patchCord4(playRaw[3], 0, mixer1, 3);
AudioConnection patchCord5(mixer1, 0, i2s1, 1);
AudioConnection patchCord6(mixer1, 0, i2s1, 0); // xy=265,212

AudioControlSGTL5000 sgtl5000_1;

const int myInput = AUDIO_INPUT_LINEIN;

String recordFilename;

void loopStation::init() {
  AudioMemory(180);
  sgtl5000_1.enable();
  sgtl5000_1.inputSelect(myInput);
  sgtl5000_1.volume(1);

  mixer1.gain(0, 1);
  mixer1.gain(1, 1);
  mixer1.gain(2, 1);
  mixer1.gain(3, 1);

  if (!SD.sdfs.begin(SdSpiConfig(SD_CS, SHARED_SPI, SD_SCK_MHZ(24))))
    Serial.println("sdinitialization failed!");

  for (size_t i = 0; i < 4; i++) {
    playRaw[i].setBufferPointer(readBuffer[i][0], readBuffer[i][1]);

    playRaw[i].setBufferSize(BUFFERSIZE, 0);
    playRaw[i].setBufferSize(BUFFERSIZE, 1);
  }

  // init SD, open all files
  // set mixer Gain
}
void loopStation::playChannel(int id) {

  Serial.print("Opening file :");
  Serial.println(playFilenames[id]);

  playFiles[id].close();

  playFiles[id] = SD.sdfs.open(playFilenames[id], O_READ | O_CREAT);

  if (!playFiles[id].isOpen())
    Serial.println("Opening file failed!");

  file_size[id] = playFiles[id].size();
  file_offset[id] = 0;

  Serial.print("Filesize:  ");
  Serial.println(file_size[id]);

  playRaw[id].setFileSize(file_size[id]);

  channelPlayState[id] = START;

  // load init buffer
  playRaw[id].play();
}

void loopStation::stopChannel(int id) {

  playRaw[id].stop();

  channelPlayState[id] = STOP;
}

void loopStation::recordChannel() {
  if (recState != FREE)
    return;

  Serial.println("startRecording");

  updateFilename();

  SD.sdfs.remove(recordFilename);

  recordFile = SD.sdfs.open(recordFilename, O_WRITE | O_CREAT);

  queue1.begin();

  recState = RECORDING;
}

void loopStation::recordChannelStop(int id) {

  if (recState == FREE)
    return;

  queue1.end();

  while (queue1.available() > 0) {
    recordFile.write((byte *)queue1.readBuffer(), 256);
    queue1.freeBuffer();
  }

  recordFile.close();
  recState = FREE;

  playFilenames[id] = recordFilename;
  playChannel(id);
}

void loopStation::serviceRoutine() {

  //*****************************************
  // Continue Recording
  if (queue1.available() >= 1) {

    uint16_t ret = recordFile.write(queue1.readBuffer(), 256);
    queue1.freeBuffer();

    if (ret != 256) {
      Serial.println("written size to SD wrong");
    }
  }
  //*****************************************
  // Continue Playing
  for (int i = 0; i < 4; i++) {
    uint16_t n = BUFFERSIZE;
    if (channelPlayState[i] == START) { // end of file??
      if (playRaw[i].getBufferState() == 1) {

        // read new buffer from sd
        n = playFiles[i].read(readBuffer[i][!playRaw[i].activeBufferID],
                              BUFFERSIZE);

        playRaw[i].bufferState = 0;
      }
    }

    if (n != BUFFERSIZE) { // end of file??
      Serial.println("EndOfFileReached");
      stopChannel(i);
    }
  }
}

void updateFilename() {
  static int counter = 0;

  recordFilename = String(String(counter) + ".RAW");

  Serial.print("newRecordFilename:  ");
  Serial.println(recordFilename);

  counter++;
}
