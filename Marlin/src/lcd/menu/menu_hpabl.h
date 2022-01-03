/**
 * Marlin 3D Printer Firmware
 * Copyright (c) 2020 MarlinFirmware [https://github.com/MarlinFirmware/Marlin]
 *
 * Based on Sprinter and grbl.
 * Copyright (c) 2011 Camiel Gubbels / Erik van der Zalm
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 */
#pragma once

#include "../../inc/MarlinConfigPre.h"
#include "../dogm/marlinui_DOGM.h"
#include "../lcdprint.h"
#include "../marlinui.h"


#define PLANE_SIZE 5 * 5

#if HAS_BUZZER
  #define _BUZZ(D,F) NOOP
#else
  #define _BUZZ(D,F) BUZZ(D,F)
#endif

class MarlinHPABL {
protected:
  static int score;
  static uint8_t hpabl_state;
  static millis_t next_frame;

  static bool hpabl_frame();

  static void draw_hpabl_over();
  static void exit_hpabl();
public:
  static void init_hpabl(const uint8_t init_state, const screenFunc_t screen);
  static void hpabl_screen();
  static void enter_hpabl();
};

typedef struct {
  uint8_t cursor_x,cursor_y;
  float delta[PLANE_SIZE];
} hpabl_data_t;
