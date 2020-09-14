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
#include "Flute.h"
#include "esp_wifi.h"

static const char* TAG = "main";

Flute* flute;

void updateParam(int id, const std::string& path, float value) {
  flute->setParamValue(path, value);
}

extern "C" void app_main() {
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

  int SR = 20000;
  int BS = 512;

  flute = new Flute(SR, BS);
  flute->start();

  MapLooper::MapLooper* mapLooper = new MapLooper::MapLooper();

  // mapLooper->addSignal("freq", 20.0f, 20000.0f, updateParam);
  // mapLooper->addSignal("bend", 0.0f, 10.0f, updateParam);
  mapLooper->addSignal("tubeLength", 0.1f, 2.0f, updateParam);
  mapLooper->addSignal("pressure", 0.0f, 1.0f, updateParam);
  mapLooper->addSignal("mouthPosition", 0.0f, 1.0f, updateParam);
  mapLooper->addSignal("vibratoFreq", 0.0f, 10.0f, updateParam);
  mapLooper->addSignal("vibratoGain", 0.0f, 1.0f, updateParam);
}
