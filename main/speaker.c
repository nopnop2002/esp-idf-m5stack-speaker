#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_err.h"
#include "esp_log.h"
#include "driver/gpio.h"

#include "speaker.h"
#include "esp32-hal-ledc.h"

extern QueueHandle_t xQueueSound;

static const char *TAG = "SPEAKER";

unsigned long millis(){
  return xTaskGetTickCount() * portTICK_RATE_MS;
}

void speaker_begin(SPEAKER_t * dev) {
  dev->_begun = true;
  dev->_volume = 8;
  ledcSetup(TONE_PIN_CHANNEL, 0, 13);
  ledcAttachPin(SPEAKER_PIN, TONE_PIN_CHANNEL);
  speaker_setBeep(dev, 1000, 100);
}

void speaker_end(SPEAKER_t * dev) {
  speaker_mute(dev);
  ledcDetachPin(SPEAKER_PIN);
  dev->_begun = false;
}

void speaker_tone(SPEAKER_t * dev, uint16_t frequency) {
  if(!dev->_begun) speaker_begin(dev);
  ledcWriteTone(TONE_PIN_CHANNEL, frequency);
  dev->_speaker_on = 1;
}

void speaker_tone_duration(SPEAKER_t * dev, uint16_t frequency, uint32_t duration) {
  speaker_tone(dev, frequency);
  dev->_count = millis() + duration;
}

void speaker_beep(SPEAKER_t * dev) {
  if(!dev->_begun) speaker_begin(dev);
  speaker_tone_duration(dev, dev->_beep_freq, dev->_beep_duration);
}

void speaker_beep_forever(SPEAKER_t * dev) {
  if(!dev->_begun) speaker_begin(dev);
  speaker_tone(dev, dev->_beep_freq);
}

void speaker_setBeep(SPEAKER_t * dev, uint16_t frequency, uint16_t duration) {
  dev->_beep_freq = frequency;
  dev->_beep_duration = duration;
}

void speaker_mute(SPEAKER_t * dev) {
  ledcWriteTone(TONE_PIN_CHANNEL, 0);
  gpio_set_level(SPEAKER_PIN, 0);
  dev->_speaker_on = 0;
}

bool speaker_update(SPEAKER_t * dev) {
  if(dev->_speaker_on) {
	if(millis() > dev->_count) {
	  speaker_mute(dev);
	  return true;
	} else {
	  return false;
	}
  } else {
	return true;
  }
}

bool speaker_status(SPEAKER_t * dev) {
  return dev->_speaker_on;
}

#if 0
void speaker_setVolume(SPEAKER_t * dev, uint8_t volume) {
  dev->_volume = 11 - volume;
}

void speaker_write(SPEAKER_t * dev, uint8_t value) {
  dacWrite(SPEAKER_PIN, value);
}

void speaker_playMusic(SPEAKER_t * dev, const uint8_t* music_data, uint16_t sample_rate) {
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

size_t makeTone(char * tone, size_t length, uint16_t * frequency, uint32_t * duration)
{
	ESP_LOGI(TAG, "tone=[%s]", tone);
	for(int i=0; i<length/2; i++) {
		char c1 = tone[i*2];
		ESP_LOGD(TAG, "c1=%c", c1);
		frequency[i] = NOTE_D0; //
		if (c1 == 'C') frequency[i] = NOTE_D1; //Do
		if (c1 == 'D') frequency[i] = NOTE_D2; //Re
		if (c1 == 'E') frequency[i] = NOTE_D3; //Mi
		if (c1 == 'F') frequency[i] = NOTE_D4; //Fa
		if (c1 == 'G') frequency[i] = NOTE_D5; //Sol
		if (c1 == 'A') frequency[i] = NOTE_D6; //Ra
		if (c1 == 'B') frequency[i] = NOTE_D7; //Si
		if (c1 == 'c') frequency[i] = NOTE_DH1; //Do
		if (c1 == 'd') frequency[i] = NOTE_DH2; //Re
		if (c1 == 'e') frequency[i] = NOTE_DH3; //Mi
		if (c1 == 'f') frequency[i] = NOTE_DH4; //Fa
		if (c1 == 'g') frequency[i] = NOTE_DH5; //Sol
		if (c1 == 'a') frequency[i] = NOTE_DH6; //Ra
		if (c1 == 'b') frequency[i] = NOTE_DH7; //Si

		char c2[2];
		c2[0] = tone[i*2+1];
		c2[1] = 0;
		int d2 = atoi(c2);
		duration[i] = d2 * 100;
		ESP_LOGD(TAG, "c2=%s d2=%d duration[i]=%u", c2, d2, duration[i]);
	}
	return (length/2);
		
}


void speaker(void *pvParameters)
{
	SPEAKER_t speaker;
	speaker_begin(&speaker);
	ESP_LOGI(TAG, "CONFIG_BEEP_FREQUENCY=%d", CONFIG_BEEP_FREQUENCY);
	ESP_LOGI(TAG, "CONFIG_BEEP_DURATION=%d", CONFIG_BEEP_DURATION);
	speaker_setBeep(&speaker, CONFIG_BEEP_FREQUENCY, CONFIG_BEEP_DURATION);

	uint16_t frequency[32];
	uint32_t duration[32];

	SOUND_t soundBuf;

	while(1) {
		xQueueReceive(xQueueSound, &soundBuf, portMAX_DELAY);
		ESP_LOGI(pcTaskGetTaskName(0),"soundBuf.command=%d", soundBuf.command);
		if (soundBuf.command == SOUND_BEEP) {
			if (speaker_status(&speaker)) {
				speaker_mute(&speaker);
			}
			speaker_beep(&speaker);
			while(1) {
				if (speaker_update(&speaker)) break;
			}

		} else if (soundBuf.command == SOUND_TONE) {
			if (speaker_status(&speaker)) {
				speaker_mute(&speaker);
			}
			size_t toneLength = makeTone(soundBuf.tone, strlen(soundBuf.tone), frequency, duration);
			ESP_LOGI(TAG, "toneLength=%d", toneLength);
			for (int i=0;i<toneLength;i++) {
				speaker_tone_duration(&speaker, frequency[i], duration[i]);
				while(1) {
					if (speaker_update(&speaker)) break;
				}
			}

		} else if (soundBuf.command == SOUND_ALARM) {
			char tone[64];
			size_t toneLength = 0;
			if (soundBuf.alarm == 0) {
				strcpy(tone, "C808");
			} else if (soundBuf.alarm == 1) {
				strcpy(tone, "E101E608");
			} else if (soundBuf.alarm == 2) {
				strcpy(tone, "G101G101G408");
			} else if (soundBuf.alarm == 3) {
				strcpy(tone, "C8c8");
			}
			toneLength = makeTone(tone, strlen(tone), frequency, duration);
			for (int i=0;i<toneLength;i++) {
				speaker_tone_duration(&speaker, frequency[i], duration[i]);
				while(1) {
					if (speaker_update(&speaker)) break;
				}
			}
			vTaskDelay(1);
			xQueueSend(xQueueSound, &soundBuf, 0);

		} else if (soundBuf.command == SOUND_MUTE) {
			if (speaker_status(&speaker)) {
				speaker_mute(&speaker);
			}
		}
	}
}
