#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/gpio.h"

#include "speaker.h"
#include "esp32-hal-ledc.h"

unsigned long millis(){
  return xTaskGetTickCount() * portTICK_RATE_MS;
}

void SPEAKER_begin(SPEAKER_t * dev) {
  dev->_begun = true;
  dev->_volume = 8;
  ledcSetup(TONE_PIN_CHANNEL, 0, 13);
  ledcAttachPin(SPEAKER_PIN, TONE_PIN_CHANNEL);
  SPEAKER_setBeep(dev, 1000, 100);
}

void SPEAKER_end(SPEAKER_t * dev) {
  SPEAKER_mute(dev);
  ledcDetachPin(SPEAKER_PIN);
  dev->_begun = false;
}

void SPEAKER_tone(SPEAKER_t * dev, uint16_t frequency) {
  if(!dev->_begun) SPEAKER_begin(dev);
  ledcWriteTone(TONE_PIN_CHANNEL, frequency);
  dev->_speaker_on = 1;
}

void SPEAKER_tone_duration(SPEAKER_t * dev, uint16_t frequency, uint32_t duration) {
  SPEAKER_tone(dev, frequency);
  dev->_count = millis() + duration;
}

void SPEAKER_beep(SPEAKER_t * dev) {
  if(!dev->_begun) SPEAKER_begin(dev);
  SPEAKER_tone_duration(dev, dev->_beep_freq, dev->_beep_duration);
}

void SPEAKER_beep_forever(SPEAKER_t * dev) {
  if(!dev->_begun) SPEAKER_begin(dev);
  SPEAKER_tone(dev, dev->_beep_freq);
}

void SPEAKER_setBeep(SPEAKER_t * dev, uint16_t frequency, uint16_t duration) {
  dev->_beep_freq = frequency;
  dev->_beep_duration = duration;
}

void SPEAKER_setVolume(SPEAKER_t * dev, uint8_t volume) {
  dev->_volume = 11 - volume;
}

void SPEAKER_mute(SPEAKER_t * dev) {
  ledcWriteTone(TONE_PIN_CHANNEL, 0);
  gpio_set_level(SPEAKER_PIN, 0);
  dev->_speaker_on = 0;
}

bool SPEAKER_update(SPEAKER_t * dev) {
  if(dev->_speaker_on) {
    if(millis() > dev->_count) {
      SPEAKER_mute(dev);
      return true;
    } else {
      return false;
    }
  } else {
    return true;
  }
}

bool SPEAKER_status(SPEAKER_t * dev) {
  return dev->_speaker_on;
}

#if 0
void SPEAKER_write(SPEAKER_t * dev, uint8_t value) {
  dacWrite(SPEAKER_PIN, value);
}

void SPEAKER_playMusic(SPEAKER_t * dev, const uint8_t* music_data, uint16_t sample_rate) {
  uint32_t length = strlen((char*)music_data);
  uint16_t delay_interval = ((uint32_t)1000000 / sample_rate);
  if(dev->_volume != 11) {
    for(int i = 0; i < length; i++) {
      dacWrite(SPEAKER_PIN, music_data[i] / dev->_volume);
      delayMicroseconds(delay_interval);
    }

    for(int t = music_data[length - 1] / dev->_volume; t >= 0; t--) {
      dacWrite(SPEAKER_PIN, t);
      delay(2);
    }
  }
  // ledcSetup(TONE_PIN_CHANNEL, 0, 13);
  ledcAttachPin(SPEAKER_PIN, TONE_PIN_CHANNEL);
}
#endif
