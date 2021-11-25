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
    pinMode(pin PWM);
    timer_dev *timer = PIN_MAP[pin].timer_device;
    uint8 channel = PIN_MAP[pin].timer_channel;
    timer_pause(timer);
    timer_set_mode(timer, channel, TIMER_OUTPUT_COMPARE); // counter
    timer_set_count(timer, 0);

     // timer_set_prescaler(timer, (uint16_t)(STEPPER_TIMER_PRESCALE - 1));
     // timer_set_reload(STEP_TIMER_DEV, 0xFFFF);
     // timer_oc_set_mode(STEP_TIMER_DEV, STEP_TIMER_CHAN, TIMER_OC_MODE_FROZEN, TIMER_OC_NO_PRELOAD); // no output pin change
     // timer_sbet_compare(STEP_TIMER_DEV, STEP_TIMER_CHAN, _MIN(hal_timer_t(HAL_TIMER_TYPE_MAX), (STEPPER_TIMER_RATE) / frequency));
     // timer_no_ARR_preload_ARPE(STEP_TIMER_DEV); // Need to be sure no preload on ARR register
     // timer_attach_interrupt(STEP_TIMER_DEV, STEP_TIMER_CHAN, stepTC_Handler);
     // timer_set_interrupt_priority(STEP_TIMER_NUM, STEP_TIMER_IRQ_PRIO);
     // timer_generate_update(STEP_TIMER_DEV);
      
    ASSERT(timer && channel);
    if (timer->regs.bas->SR & 1) set_pwm_frequency(pin, 1000);
    timer_set_reload(timer, 0xFF );
    timer_set_compare(timer, channel, v);
    timer_resume(timer);
    uint8_t timer_i = 0;
    for (uint8_t i = 0; i < 14; i++) if (timer == get_timer_dev(i)) timer_i = i+1;      

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

  // Protect used timers
  // if (timer == get_timer_dev(TEMP_TIMER_NUM)) return;
  // if (timer == get_timer_dev(STEP_TIMER_NUM)) return;
  // #if PULSE_TIMER_NUM != STEP_TIMER_NUM
  //   if (timer == get_timer_dev(PULSE_TIMER_NUM)) return;
  // #endif

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
