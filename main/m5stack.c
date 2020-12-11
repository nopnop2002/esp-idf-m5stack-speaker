/* M5stack Task

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_err.h"
#include "esp_log.h"
#include "driver/gpio.h"

#include "ili9340.h"
#include "fontx.h"
#include "cmd.h"
#include "speaker.h"

// M5Stack stuff
#define SCREEN_WIDTH	320
#define SCREEN_HEIGHT	240
#define CS_GPIO			14
#define DC_GPIO			27
#define RESET_GPIO		33
#define BL_GPIO			32
#define DISPLAY_LENGTH	26
#define GPIO_INPUT_A	GPIO_NUM_39
#define GPIO_INPUT_B	GPIO_NUM_38
#define GPIO_INPUT_C	GPIO_NUM_37

extern QueueHandle_t xQueueCmd;
extern QueueHandle_t xQueueSound;

// Left Button Monitoring
void buttonA(void *pvParameters)
{
	ESP_LOGI(pcTaskGetTaskName(0), "Start");
	CMD_t cmdBuf;
	//cmdBuf.command = CMD_LEFT;
	cmdBuf.taskHandle = xTaskGetCurrentTaskHandle();

	// set the GPIO as a input
	gpio_pad_select_gpio(GPIO_INPUT_A);
	gpio_set_direction(GPIO_INPUT_A, GPIO_MODE_DEF_INPUT);

	while(1) {
		int level = gpio_get_level(GPIO_INPUT_A);
		if (level == 0) {
			ESP_LOGI(pcTaskGetTaskName(0), "Push Button");
			TickType_t startTick = xTaskGetTickCount();
			while(1) {
				level = gpio_get_level(GPIO_INPUT_A);
				if (level == 1) break;
				vTaskDelay(1);
			}
			TickType_t endTick = xTaskGetTickCount();
			TickType_t diffTick = endTick-startTick;
			cmdBuf.command = CMD_LEFT;
			if (diffTick > 200) cmdBuf.command = CMD_LONG_LEFT;
			xQueueSend(xQueueCmd, &cmdBuf, 0);
		}
		vTaskDelay(1);
	}
}

// Middle Button Monitoring
void buttonB(void *pvParameters)
{
	ESP_LOGI(pcTaskGetTaskName(0), "Start");
	CMD_t cmdBuf;
	//cmdBuf.command = CMD_MIDDLE;
	cmdBuf.taskHandle = xTaskGetCurrentTaskHandle();

	// set the GPIO as a input
	gpio_pad_select_gpio(GPIO_INPUT_B);
	gpio_set_direction(GPIO_INPUT_B, GPIO_MODE_DEF_INPUT);

	while(1) {
		int level = gpio_get_level(GPIO_INPUT_B);
		if (level == 0) {
			ESP_LOGI(pcTaskGetTaskName(0), "Push Button");
			TickType_t startTick = xTaskGetTickCount();
			while(1) {
				level = gpio_get_level(GPIO_INPUT_B);
				if (level == 1) break;
				vTaskDelay(1);
			}
			TickType_t endTick = xTaskGetTickCount();
			TickType_t diffTick = endTick-startTick;
			cmdBuf.command = CMD_MIDDLE;
			if (diffTick > 200) cmdBuf.command = CMD_LONG_MIDDLE;
			xQueueSend(xQueueCmd, &cmdBuf, 0);
		}
		vTaskDelay(1);
	}
}

// Right Button Monitoring
void buttonC(void *pvParameters)
{
	ESP_LOGI(pcTaskGetTaskName(0), "Start");
	CMD_t cmdBuf;
	//cmdBuf.command = CMD_RIGHT;
	cmdBuf.taskHandle = xTaskGetCurrentTaskHandle();

	// set the GPIO as a input
	gpio_pad_select_gpio(GPIO_INPUT_C);
	gpio_set_direction(GPIO_INPUT_C, GPIO_MODE_DEF_INPUT);

	while(1) {
		int level = gpio_get_level(GPIO_INPUT_C);
		if (level == 0) {
			ESP_LOGI(pcTaskGetTaskName(0), "Push Button");
			TickType_t startTick = xTaskGetTickCount();
			while(1) {
				level = gpio_get_level(GPIO_INPUT_C);
				if (level == 1) break;
				vTaskDelay(1);
			}
			TickType_t endTick = xTaskGetTickCount();
			TickType_t diffTick = endTick-startTick;
			cmdBuf.command = CMD_RIGHT;
			if (diffTick > 200) cmdBuf.command = CMD_LONG_RIGHT;
			xQueueSend(xQueueCmd, &cmdBuf, 0);
		}
		vTaskDelay(1);
	}
}

#define CONFIG_ESP_FONT_GOTHIC 1
#define CONFIG_ESP_FONT_MINCYO 0

void tft(void *pvParameters)
{
	ESP_LOGI(pcTaskGetTaskName(0), "Start");

	// Set font file
	FontxFile fx[2];
#if CONFIG_ESP_FONT_GOTHIC
	InitFontx(fx,"/fonts/ILGH24XB.FNT",""); // 12x24Dot Gothic
#endif
#if CONFIG_ESP_FONT_MINCYO
	InitFontx(fx,"/fonts/ILMH24XB.FNT",""); // 12x24Dot Mincyo
#endif

	// Get font width & height
	uint8_t buffer[FontxGlyphBufSize];
	uint8_t fontWidth;
	uint8_t fontHeight;
	GetFontx(fx, 0, buffer, &fontWidth, &fontHeight);
	ESP_LOGI(pcTaskGetTaskName(0), "fontWidth=%d fontHeight=%d",fontWidth,fontHeight);

	// Setup Screen
	TFT_t dev;
	spi_master_init(&dev, CS_GPIO, DC_GPIO, RESET_GPIO, BL_GPIO);
	lcdInit(&dev, 0x9341, SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0);
	ESP_LOGI(pcTaskGetTaskName(0), "Setup Screen done");

#if 0
	int lines = (SCREEN_HEIGHT - fontHeight) / fontHeight;
	ESP_LOGD(pcTaskGetTaskName(0), "SCREEN_HEIGHT=%d fontHeight=%d lines=%d", SCREEN_HEIGHT, fontHeight, lines);
	int ymax = (lines+1) * fontHeight;
	ESP_LOGD(pcTaskGetTaskName(0), "ymax=%d",ymax);
#endif

	// Clear Screen
	lcdFillScreen(&dev, BLACK);
	lcdSetFontDirection(&dev, 0);

	// Reset scroll area
	lcdSetScrollArea(&dev, 0, 0x0140, 0);

	// Show text
	uint8_t ascii[44];
	uint16_t ypos;
	ypos = fontHeight-1;
	strcpy((char *)ascii, "M5Stack Speaker");
	lcdDrawString(&dev, fx, 0, ypos, ascii, YELLOW);
	ypos = (fontHeight*3)-1;
	strcpy((char *)ascii, "Left button is beep.");
	lcdDrawString(&dev, fx, 0, ypos, ascii, YELLOW);
	ypos = (fontHeight*5)-1;
	strcpy((char *)ascii, "Middle button play Doremi.");
	lcdDrawString(&dev, fx, 0, ypos, ascii, YELLOW);
	ypos = (fontHeight*7)-1;
	strcpy((char *)ascii, "Right button start alarm.");
	lcdDrawString(&dev, fx, 0, ypos, ascii, YELLOW);
	ypos = (fontHeight*8)-1;
	strcpy((char *)ascii, "Stops when pressed again.");
	lcdDrawString(&dev, fx, 0, ypos, ascii, YELLOW);

	CMD_t cmdBuf;
	SOUND_t soundBuf;
	soundBuf.taskHandle = xTaskGetCurrentTaskHandle();
	int alarmType = 0;
	bool alarmStatus = false;

	while(1) {
		xQueueReceive(xQueueCmd, &cmdBuf, portMAX_DELAY);
		ESP_LOGI(pcTaskGetTaskName(0),"cmdBuf.command=%d", cmdBuf.command);
		if (cmdBuf.command == CMD_LEFT) {
			ESP_LOGI(pcTaskGetTaskName(0),"LEFT Button");
			soundBuf.command = SOUND_BEEP;
			xQueueSend(xQueueSound, &soundBuf, 0);

		} else if (cmdBuf.command == CMD_LONG_LEFT) {
			ESP_LOGI(pcTaskGetTaskName(0),"LONG LEFT Button");

		} else if (cmdBuf.command == CMD_MIDDLE) {
			ESP_LOGI(pcTaskGetTaskName(0),"MIDDLE Button");
			soundBuf.command = SOUND_TONE;
			strcpy(soundBuf.tone, "C2D2E2F2G2A2B2c2");
			xQueueSend(xQueueSound, &soundBuf, 0);

		} else if (cmdBuf.command == CMD_LONG_MIDDLE) {
			ESP_LOGI(pcTaskGetTaskName(0),"LONG MIDDLE Button");

		} else if (cmdBuf.command == CMD_RIGHT) {
			ESP_LOGI(pcTaskGetTaskName(0),"RIGHT Button");
			if (alarmStatus) {
				soundBuf.command = SOUND_MUTE;
				alarmStatus = false;
			} else {
				soundBuf.command = SOUND_ALARM;
				soundBuf.alarm = alarmType;
				alarmStatus = true;
				alarmType++;
				if (alarmType == 4) alarmType = 0;
			}
			xQueueSend(xQueueSound, &soundBuf, 0);

		} else if (cmdBuf.command == CMD_LONG_RIGHT) {
			ESP_LOGI(pcTaskGetTaskName(0),"LONG RIGHT Button");

		}
	}

	// nerver reach
	while (1) {
		vTaskDelay(2000 / portTICK_PERIOD_MS);
	}
}
