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
#ifdef __STM32F1__

#include "../../inc/MarlinConfigPre.h"

#include <pwm.h>
#include "HAL.h"
#include "timers.h"

static uint16_t timer_freq[TIMER_NUM];

void set_pwm_duty(const pin_t pin, const uint16_t v, const uint16_t v_size/*=255*/, const bool invert/*=false*/) {
  if (!PWM_PIN(pin)) return;
    uint16_t duty = v;
    timer_dev *timer = PIN_MAP[pin].timer_device;
    uint8_t channel = PIN_MAP[pin].timer_channel;

    if ((timer->regs.bas->SR & TIMER_CR1_CEN)) set_pwm_frequency(pin, PWM_FREQUENCY); // Unconfigured? Set default freq.
    if (invert) duty = v_size - duty;

    uint8_t timer_index = 0;
    for (uint8_t i = 0; i < 14; i++) if (timer == get_timer_dev(i)) { timer_index = i; break; }

    if (timer_freq[timer_index] == 0) set_pwm_frequency(pin, PWM_FREQUENCY);

    timer_pause(timer);
    timer_set_mode(timer, channel, TIMER_PWM); // PWM Output Mode
    timer_set_count(timer, 0);
    timer_set_compare(timer, channel, duty / timer_freq[timer_index]);
    timer_resume(timer);



  // timer_dev *timer = PIN_MAP[pin].timer_device;
  // if (!(timer->regs.bas->SR & TIMER_CR1_CEN))   // Ensure the timer is enabled
  //   set_pwm_frequency(pin, PWM_FREQUENCY);
  // uint16_t max_val = timer->regs.bas->ARR * v / v_size;
  // if (invert) max_val = v_size - max_val;
  // pwmWrite(pin, max_val);


    uint8_t timer_i = 0;
    for (uint8_t i = 0; i < 14; i++) if (timer == get_timer_dev(i)) timer_i = i;      
    SERIAL_ECHO_MSG("");
    SERIAL_ECHO_MSG("TIMER_NO: ", timer_i );
    SERIAL_ECHO_MSG("TIMER_DUTY: ", v );
    SERIAL_ECHO_MSG("TIMER_CHANNEL: ", channel);
    SERIAL_ECHO_MSG("TIMER_CR1_ARPE: ", timer->regs.bas->CR1 & 7 );
    SERIAL_ECHO_MSG("TIMER_ARR: ", timer->regs.bas->ARR );
    SERIAL_ECHO_MSG("TIMER_PSC: ", timer->regs.bas->PSC );
    SERIAL_ECHO_MSG("TIMER_CR1_CEN: ", timer->regs.bas->SR & 1 );

}

void set_pwm_frequency(const pin_t pin, int f_desired) {
  if (!PWM_PIN(pin)) return;                    // Don't proceed if no hardware timer

  timer_dev *timer = PIN_MAP[pin].timer_device;
  uint8_t channel = PIN_MAP[pin].timer_channel;
  
  uint8_t timer_index = 0;
  for (uint8_t i = 0; i < 14; i++) if (timer == get_timer_dev(i)) timer_index = i;

  timer_freq[timer_index] = f_desired;

  //Protect used timers
  if (timer == TEMP_TIMER_DEV) return;
  if (timer == STEP_TIMER_DEV) return;
  #if PULSE_TIMER_NUM != STEP_TIMER_NUM
    if (timer == PULSE_TIMER_DEV) return;
  #endif

  if (!(timer->regs.bas->SR & TIMER_CR1_CEN))   // Ensure the timer is enabled
    timer_init(timer);

  timer_set_mode(timer, channel, TIMER_PWM);
  uint16_t preload = 255;                       // Lock 255 PWM resolution for high frequencies
  int32_t prescaler = (HAL_TIMER_RATE) / (preload + 1) / f_desired - 1;
  if (prescaler > 65535) {                      // For low frequencies increase prescaler
    prescaler = 65535;
    preload = (HAL_TIMER_RATE) / (prescaler + 1) / f_desired - 1;
  }
  if (prescaler < 0) return;                    // Too high frequency
  timer_set_reload(timer, preload);
  timer_set_prescaler(timer, prescaler);
}

#endif // __STM32F1__
