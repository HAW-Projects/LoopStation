/* Audio Library for Teensy 3.X
 * Copyright (c) 2014, Paul Stoffregen, paul@pjrc.com
 *
 * Development of this audio library was funded by PJRC.COM, LLC by sales of
 * Teensy and Audio Adaptor boards.  Please support PJRC's efforts to develop
 * open source software by purchasing Teensy or other PJRC products.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice, development funding notice, and this permission
 * notice shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "play_sd_buffer.h"
#include "spi_interrupt.h"
#include <Arduino.h>

void AudioPlayBuff::begin(void) { playing = false; }

bool AudioPlayBuff::play(void) {

  // AudioStartUsingSPI();
  file_offset = 0;
  playing = true;
  buffer_offset = 0;
  bufferState = 1;
}

void AudioPlayBuff::stop(void) {

  playing = false;
  // AudioStopUsingSPI();
}

void AudioPlayBuff::setBufferPointer(byte *buffer0, byte *buffer1) {
  this->buffer[0] = buffer0;
  this->buffer[1] = buffer1;

  Serial.println("register pointer to buffer");
}

void AudioPlayBuff::setBufferSize(uint16_t bufferSize, byte bufferID) {

  this->bufferSize[bufferID] = bufferSize;
  // buffer_offset = 0;
  // bufferState = 0;
}

void AudioPlayBuff::update(void) {
  unsigned int i, n;
  audio_block_t *block;

  // only update if we're playing
  if (!playing)
    return;

  // allocate the audio blocks to transmit
  block = allocate();
  if (block == NULL)
    return;

  // if (!bufferState) {
  // we can read more data from the file...
  // n = rawfile.read(block->data, AUDIO_BLOCK_SAMPLES * 2);
  memcpy(block->data, (buffer[activeBufferID] + buffer_offset),
         AUDIO_BLOCK_SAMPLES * 2);

  n = AUDIO_BLOCK_SAMPLES * 2;

  buffer_offset += n;
  file_offset += n;

  for (i = n / 2; i < AUDIO_BLOCK_SAMPLES; i++) {
    block->data[i] = 0;
  }

  transmit(block);

  if (buffer_offset == bufferSize[activeBufferID]) {
    bufferState = 1;
    buffer_offset = 0;
    activeBufferID = !activeBufferID;
  }
  // }
  release(block);
}

void AudioPlayBuff::setFileSize(uint32_t fileSize) { file_size = fileSize; }

#define B2M                                                                    \
  (uint32_t)((double)4294967296000.0 / AUDIO_SAMPLE_RATE_EXACT /               \
             2.0) // 97352592

uint32_t AudioPlayBuff::positionMillis(void) {
  return ((uint64_t)file_offset * B2M) >> 32;
}

uint32_t AudioPlayBuff::lengthMillis(void) {
  return ((uint64_t)file_size * B2M) >> 32;
}
