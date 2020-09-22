/*
 MapLooper - Embedded Live-Looping Tools for Digital Musical Instruments
 Copyright (C) 2020 Mathias Bredholt

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <https://www.gnu.org/licenses/>.

*/

// #include "Flute/Flute.h"
#include "Flute.h"
#include "MapLooper/MapLooper.hpp"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_wifi.h"
#include "board.h"
#include "es8388.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs_flash.h"

static const char* TAG = "main";

void createFluteSignals(mpr_dev dev, Flute* flute) {
  mpr_sig sigPressure = mpr_sig_new(
      dev, MPR_DIR_IN, "flute/pressure", 1, MPR_FLT, 0, 0, 0, 0,
      [](mpr_sig sig, mpr_sig_evt evt, mpr_id inst, int length, mpr_type type,
         const void* value, mpr_time time) {
        Flute* flute = (Flute*)mpr_obj_get_prop_as_ptr(sig, MPR_PROP_DATA, 0);
        flute->setParamValue("pressure", *(float*)value);
      },
      MPR_SIG_UPDATE);
  mpr_obj_set_prop(sigPressure, MPR_PROP_DATA, 0, 1, MPR_PTR, flute, 0);

  mpr_sig sigTubeLength = mpr_sig_new(
      dev, MPR_DIR_IN, "flute/tubeLength", 1, MPR_FLT, 0, 0, 0, 0,
      [](mpr_sig sig, mpr_sig_evt evt, mpr_id inst, int length, mpr_type type,
         const void* value, mpr_time time) {
        Flute* flute = (Flute*)mpr_obj_get_prop_as_ptr(sig, MPR_PROP_DATA, 0);
        flute->setParamValue("tubeLength", *(float*)value);
      },
      MPR_SIG_UPDATE);
  mpr_obj_set_prop(sigTubeLength, MPR_PROP_DATA, 0, 1, MPR_PTR, flute, 0);
}

extern "C" void app_main() {
  // Connect to WiFi
  ESP_ERROR_CHECK(nvs_flash_init());
  ESP_ERROR_CHECK(esp_netif_init());
  ESP_ERROR_CHECK(esp_event_loop_create_default());

  esp_netif_create_default_wifi_ap();

  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  ESP_ERROR_CHECK(esp_wifi_init(&cfg));

  wifi_config_t wifi_config = {
      .ap = {"MapLooper", "mappings", .authmode = WIFI_AUTH_WPA2_PSK,
             .max_connection = 4},
  };

  ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
  ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_AP, &wifi_config));
  ESP_ERROR_CHECK(esp_wifi_start());

  audio_board_handle_t board_handle = audio_board_init();
  audio_hal_ctrl_codec(board_handle->audio_hal, AUDIO_HAL_CODEC_MODE_BOTH,
                       AUDIO_HAL_CTRL_START);
  audio_hal_set_volume(board_handle->audio_hal, 50);

  int SR = 44100;
  int BS = 256;

  Flute* flute = new Flute(SR, BS);
  flute->start();

  flute->setParamValue("pressure", 0.8);

  MapLooper::MapLooper* mapLooper = new MapLooper::MapLooper();

  createFluteSignals(mapLooper->getMapperDevice(), flute);

  MapLooper::Loop* loop =
      mapLooper->createLoop("test", MPR_FLT, 1, "slider1", "pressure");
  loop->mapMixTo("button1");

  xTaskCreate(
      [](void* userParam) {
        MapLooper::MapLooper* m =
            reinterpret_cast<MapLooper::MapLooper*>(userParam);
        for (;;) {
          m->update();
          vTaskDelay(1);
        }
      },
      "MapLooper", 16384, mapLooper, 3, nullptr);

  vTaskDelay(portMAX_DELAY);
}
