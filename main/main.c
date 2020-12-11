/*
   This code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_system.h"
#include "esp_vfs.h"
#include "esp_spiffs.h"
#include "esp_log.h"

#include "cmd.h"

QueueHandle_t xQueueCmd;

static const char *TAG = "MAIN";

static void SPIFFS_Directory(char * path) {
	DIR* dir = opendir(path);
	assert(dir != NULL);
	while (true) {
		struct dirent*pe = readdir(dir);
		if (!pe) break;
		ESP_LOGI(TAG,"d_name=%s/%s d_ino=%d d_type=%x", path, pe->d_name,pe->d_ino, pe->d_type);
	}
	closedir(dir);
}

esp_err_t SPIFFS_Mount(char * path, char * label, int max_files) {
	esp_vfs_spiffs_conf_t esp_vfs_spiffs_conf = {
		.base_path = path,
		.partition_label = label,
		.max_files = max_files,
		.format_if_mount_failed =true
	};

	// Use settings defined above toinitialize and mount SPIFFS filesystem.
	// Note: esp_vfs_spiffs_register is anall-in-one convenience function.
	esp_err_t ret = esp_vfs_spiffs_register(&esp_vfs_spiffs_conf);
	if (ret != ESP_OK) {
		if (ret ==ESP_FAIL) {
			ESP_LOGE(TAG, "Failed to mount or format filesystem");
		} else if (ret== ESP_ERR_NOT_FOUND) {
			ESP_LOGE(TAG, "Failed to find SPIFFS partition");
		} else {
		}
		return ret;
	}

	size_t total = 0, used = 0;
	ret = esp_spiffs_info(esp_vfs_spiffs_conf.partition_label, &total, &used);
	if (ret != ESP_OK) {
		ESP_LOGE(TAG,"Failed to get SPIFFS partition information (%s)",esp_err_to_name(ret));
	} else {
		ESP_LOGI(TAG,"Partition size: total: %d, used: %d", total, used);
	}

	if (ret == ESP_OK) {
		ESP_LOGI(TAG, "Mount %s to %s success", label, path);
		SPIFFS_Directory(path);
	}
	return ret;
}

void buttonA(void *pvParameters);
void buttonB(void *pvParameters);
void buttonC(void *pvParameters);
void tft(void *pvParameters);

void app_main(void)
{
	ESP_LOGI(TAG, "Mount storage0 to /fonts");
	if (SPIFFS_Mount("/fonts", "storage0", 6) != ESP_OK)
	{
		ESP_LOGE(TAG, "SPIFFS mount failed");
		while(1) { vTaskDelay(1); }
	}

#if 0
	ESP_LOGI(TAG, "Mount storage1 to /images");
	if (SPIFFS_Mount("/images", "storage1", 6) != ESP_OK)
	{
		ESP_LOGE(TAG, "SPIFFS mount failed");
		while(1) { vTaskDelay(1); }
	}
	ESP_LOGI(TAG, "Initializing SPIFFS done");
#endif


	// Create Queue
	xQueueCmd = xQueueCreate( 10, sizeof(CMD_t) );
	configASSERT( xQueueCmd );

	// Create Task
	xTaskCreate(buttonA, "BUTTON1", 1024*2, NULL, 2, NULL);
	xTaskCreate(buttonB, "BUTTON2", 1024*2, NULL, 2, NULL);
	xTaskCreate(buttonC, "BUTTON3", 1024*2, NULL, 2, NULL);
	xTaskCreate(tft, "TFT", 1024*8, NULL, 5, NULL);
}
