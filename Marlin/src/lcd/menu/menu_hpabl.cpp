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

#include "../../inc/MarlinConfigPre.h"
#include "menu_item.h"

#if ENABLED(HPABL)
#include "menu_hpabl.h"


int MarlinHPABL::score;
uint8_t MarlinHPABL::hpabl_state;
millis_t MarlinHPABL::next_frame;

hpabl_data_t hpabl;

bool MarlinHPABL::hpabl_frame() {
  static int8_t slew;
  if (ui.first_page) slew = 2;
  ui.refresh(LCDVIEW_CALL_NO_REDRAW); // Refresh as often as possible
  return (hpabl_state && slew-- > 0);
}

void MarlinHPABL::draw_hpabl_over() {
  constexpr int8_t gowide = (MENU_FONT_WIDTH) * 9,
                   gohigh = MENU_FONT_ASCENT - 3,
                       lx = (LCD_PIXEL_WIDTH - gowide) / 2,
                       ly = (LCD_PIXEL_HEIGHT + gohigh) / 2;
  if (PAGE_CONTAINS(ly - gohigh - 1, ly + 1)) {
    u8g.setColorIndex(0);
    u8g.drawBox(lx - 1, ly - gohigh - 1, gowide + 2, gohigh + 2);
    u8g.setColorIndex(1);
    if (ui.get_blink()) lcd_put_u8str(lx, ly, F("Complete"));
  }
}

void MarlinHPABL::init_hpabl(const uint8_t init_state, const screenFunc_t screen) {
  score = 0;
  hpabl_state = init_state;
  ui.goto_screen(screen);
  ui.defer_status_screen();
}

void MarlinHPABL::exit_hpabl() {
  ui.goto_previous_screen_no_defer();
}


void MarlinHPABL::hpabl_screen() {
  if (hpabl_frame()) { 
    // Update Cursor Position
    hpabl.cursor_y = constrain(ui.encoderPosition, 0, LCD_PIXEL_WIDTH );
    ui.encoderPosition = hpabl.cursor_y;

    if (hpabl_state) do {

    } while (false);
  
    u8g.setColorIndex(1);
    }


    PGM_P const ohs1 = PSTR(" 1.1 1.4 1.3 1.8 0.5\0");
    lcd_put_u8str_P(0,10,ohs1);
    PGM_P const ohs2 = PSTR(" 1.0 1.7 1.4 1.6 0.2\0");
    lcd_put_u8str_P(0,20,ohs2);
    PGM_P const ohs3 = PSTR(" 0.4 1.6 0.7 1.8 1.5\0");
    lcd_put_u8str_P(0,30,ohs3);
    PGM_P const ohs4 = PSTR(" 1.5 1.4 1.1 1.4 1.4\0");
    lcd_put_u8str_P(0,40,ohs4);
    PGM_P const ohs5 = PSTR(" 1.6 1.3 1.2 1.4 1.0\0");
    lcd_put_u8str_P(0,50,ohs5); 


  // A click always exits this screen
  if (ui.use_click()) exit_hpabl();
}

#define SCREEN_M ((LCD_PIXEL_WIDTH) / 2)

void MarlinHPABL::enter_hpabl() {
  init_hpabl(2, hpabl_screen); 
  ui.encoderPosition = 0;
}

#endif // HPABL
