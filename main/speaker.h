#ifndef MAIN_SPEAKER_H_
#define MAIN_SPEAKER_H_

#include <stdint.h>
#include <stdbool.h>

#define SOUND_BEEP  100
#define SOUND_TONE  200
#define SOUND_ALARM 300
#define SOUND_MUTE  400
#define SOUND_HALT  900

typedef struct {
	uint16_t command;
	char tone[64];
	uint16_t alarm;
	TaskHandle_t taskHandle;
} SOUND_t;

typedef struct {
	uint32_t _count;
	uint8_t _volume;
	uint16_t _beep_duration;
	uint16_t _beep_freq;
	bool _begun;
	bool _speaker_on;
} SPEAKER_t;

#define NOTE_D0 -1
#define NOTE_D1 294
#define NOTE_D2 330
#define NOTE_D3 350
#define NOTE_D4 393
#define NOTE_D5 441
#define NOTE_D6 495
#define NOTE_D7 556

#define NOTE_DL1 147
#define NOTE_DL2 165
#define NOTE_DL3 175
#define NOTE_DL4 196
#define NOTE_DL5 221
#define NOTE_DL6 248
#define NOTE_DL7 278

#define NOTE_DH1 589
#define NOTE_DH2 661
#define NOTE_DH3 700
#define NOTE_DH4 786
#define NOTE_DH5 882
#define NOTE_DH6 990
#define NOTE_DH7 112

#define TONE_PIN_CHANNEL 0
#define SPEAKER_PIN 25

void speaker_begin(SPEAKER_t * dev);
void speaker_end(SPEAKER_t * dev);
void speaker_tone(SPEAKER_t * dev, uint16_t frequency);
void speaker_tone_duration(SPEAKER_t * dev, uint16_t frequency, uint32_t duration);
void speaker_beep(SPEAKER_t * dev);
void speaker_beep_forever(SPEAKER_t * dev);
void speaker_setBeep(SPEAKER_t * dev, uint16_t frequency, uint16_t duration);
void speaker_mute(SPEAKER_t * dev);
bool speaker_update(SPEAKER_t * dev);
bool speaker_status(SPEAKER_t * dev);

#if 0
void speaker_setVolume(SPEAKER_t * dev, uint8_t volume);
void speaker_write(SPEAKER_t * dev, uint8_t value);
void speaker_playMusic(SPEAKER_t * dev, const uint8_t *music_data, uint16_t sample_rate);
#endif

#endif
