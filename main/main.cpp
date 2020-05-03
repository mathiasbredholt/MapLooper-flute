/*
           __
  /\/\    / /
 /    \  / /
/ /\/\ \/ /___
\/    \/\____/
MapLooper
(c) Mathias Bredholt 2020

*-0-*-0-*-0-*-0-*-0-*-0-*-0-*-0-*-0-*

main.cpp
main

*/

#include "MapLooper/MapLooper.hpp"
#include "board.h"
#include "es8388.h"
#include "esp_event.h"
#include "esp_log.h"
#include "freertos/task.h"
#include "nvs_flash.h"
#include "protocol_examples_common.h"
#include "Flute/Flute.h"
#include "esp_wifi.h"

static const char* TAG = "main";

Flute* flute;

void updateParam(const char* name, float value) {
  flute->setParamValue(name, value);
}

void mapLooperTask(void* user_param) {
  mpr_dev dev = mpr_dev_new("MapLooper", NULL);
  MapLooper::MapLooper* app = new MapLooper::MapLooper(&dev);

  int SR = 20000;
  int BS = 512;

  flute = new Flute(SR, BS);
  flute->start();

  app->mapper.addSignal("freq", 20.0f, 20000.0f, updateParam);
  app->mapper.addSignal("bend", 0.0f, 10.0f, updateParam);
  app->mapper.addSignal("gate", 0.0f, 1.0f, updateParam);
  app->mapper.addSignal("mouthPosition", 0.0f, 1.0f, updateParam);
  app->mapper.addSignal("vibratoFreq", 0.0f, 10.0f, updateParam);
  app->mapper.addSignal("vibratoGain", 0.0f, 1.0f, updateParam);


  flute->setParamValue("gate", 1);
  // flute->keyOn(0, 60, 100);

  // app->midiOut.note_on = [&](uint8_t pitch, uint8_t velocity, uint8_t channel) {
  //   flute->keyOn(channel, pitch, velocity);
  // };

  // app->midiOut.note_off = [&](uint8_t pitch, uint8_t channel) {
  //   flute->keyOff(channel, pitch, 0);
  // };

  TickType_t xLastWakeTime = xTaskGetTickCount();
  const TickType_t xFrequency = 5;
  while (true) {
    vTaskDelayUntil(&xLastWakeTime, xFrequency);
    app->update();
  }
}

extern "C" void app_main() {
  // ESP_LOGI(TAG, "MapLooper-flute v%s", esp_ota_get_app_description()->version);
  // ESP_LOGI(TAG, "(c) Mathias Bredholt 2020");

  // Connect to WiFi
  ESP_ERROR_CHECK(nvs_flash_init());
  ESP_ERROR_CHECK(esp_netif_init());
  ESP_ERROR_CHECK(esp_event_loop_create_default());
  ESP_ERROR_CHECK(example_connect());

  ESP_LOGI(TAG, "[ 2 ] Start audio codec chip");
  audio_board_handle_t board_handle = audio_board_init();
  audio_hal_ctrl_codec(board_handle->audio_hal, AUDIO_HAL_CODEC_MODE_BOTH,
                       AUDIO_HAL_CTRL_START);
  audio_hal_set_volume(board_handle->audio_hal, 90);

  xTaskCreate(mapLooperTask, "MapLooper", 4096, NULL, 30, NULL);
}
