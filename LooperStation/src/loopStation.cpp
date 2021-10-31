#include "loopStation.hpp"

AudioInputI2S i2s2;        // xy=651,667
AudioAnalyzePeak peak1;    // xy=824,655
AudioRecordQueue queue1;   // xy=835,560
AudioPlaySdRaw playRaw[4]; // xy=716,596

AudioMixer4 mixer1;  // xy=946,601
AudioOutputI2S i2s1; // xy=1169,670
AudioConnection qpatchCord7(i2s2, 0, queue1, 0);
AudioConnection patchCord8(i2s2, 0, peak1, 0);
AudioConnection patchCord2(playRaw[1], 0, mixer1, 0);
AudioConnection patchCord1(playRaw[2], 0, mixer1, 1);
AudioConnection patchCord3(playRaw[3], 0, mixer1, 2);
AudioConnection patchCord4(playRaw[4], 0, mixer1, 3);
AudioConnection patchCord5(mixer1, 0, i2s1, 1);
AudioConnection patchCord6(mixer1, 0, i2s1, 0); // xy=265,212

AudioControlSGTL5000 sgtl5000_1;
const int myInput = AUDIO_INPUT_LINEIN;

void loopStation::init() {
  sgtl5000_1.enable();
  sgtl5000_1.inputSelect(myInput);
  sgtl5000_1.volume(0.5);

  mixer1.gain(0, 0.5);
  mixer1.gain(1, 0.5);
  mixer1.gain(2, 0.5);
  mixer1.gain(3, 0.5);

  if (!SD.sdfs.begin(SdSpiConfig(SD_CS, SHARED_SPI, SD_SCK_MHZ(24))))
    Serial.println("sdinitialization failed!");

  // init SD, open all files
  // set mixer Gain
}
void loopStation::playChannel(int id) {
  playFiles[id].close();
  playFiles[id] = SD.sdfs.open(playFilenames[id], O_READ | O_CREAT);
  channelPlayState[id] = START;
}

void loopStation::stopChannel(int id) {
  playFiles[id].close();
  channelPlayState[id] = STOP;
}

void loopStation::recordChannel() {
  if (recState != FREE)
    return;

  Serial.println("startRecording");

  updateFilename();

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

  // continous play record
  // handle buffer
}
