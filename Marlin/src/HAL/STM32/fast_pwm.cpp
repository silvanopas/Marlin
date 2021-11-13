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

#include "../platforms.h"

#ifdef HAL_STM32

#include "../../inc/MarlinConfig.h"
#include "timers.h"

void set_pwm_duty(const pin_t pin, const uint16_t v, const uint16_t v_size/*=255*/, const bool invert/*=false*/) {
  if (!PWM_PIN(pin)) return;            // Don't proceed if no hardware timer
  bool needs_freq;
  PinName pin_name = digitalPinToPinName(pin);
  TIM_TypeDef *Instance = (TIM_TypeDef *)pinmap_peripheral(pin_name, PinMap_PWM);
  HardwareTimer *HT;
  TimerModes_t previousMode;
  uint32_t index = get_timer_index(Instance);
  if (HardwareTimer_Handle[index] == NULL) {
    HardwareTimer_Handle[index]->__this = new HardwareTimer((TIM_TypeDef *)pinmap_peripheral(pin_name, PinMap_PWM));
    needs_freq = true;
  }
  HT = (HardwareTimer *)(HardwareTimer_Handle[index]->__this);
  uint32_t channel = STM_PIN_CHANNEL(pinmap_function(pin_name, PinMap_PWM));
  uint16_t adj_val = Instance->ARR * v / v_size;
  if (invert) adj_val = Instance->ARR - adj_val;

  previousMode = HT->getMode(channel);
  if (previousMode != TIMER_OUTPUT_COMPARE_PWM1) {
    HT->setMode(channel, TIMER_OUTPUT_COMPARE_PWM1, pin);
  }
  if (needs_freq) HT->setOverflow(PWM_FREQUENCY, HERTZ_FORMAT);
  HT->setCaptureCompare(channel, adj_val, RESOLUTION_8B_COMPARE_FORMAT);
  if (previousMode != TIMER_OUTPUT_COMPARE_PWM1) {
    HT->resume();
  }
}

#if NEEDS_HARDWARE_PWM

  void set_pwm_frequency(const pin_t pin, int f_desired) {
    if (!PWM_PIN(pin)) return;            // Don't proceed if no hardware timer

    PinName pin_name = digitalPinToPinName(pin);
    TIM_TypeDef *Instance = (TIM_TypeDef *)pinmap_peripheral(pin_name, PinMap_PWM); // Get HAL timer instance

    LOOP_S_L_N(i, 0, NUM_HARDWARE_TIMERS) // Protect used timers
      if (timer_instance[i] && timer_instance[i]->getHandle()->Instance == Instance)
        return;

    pwm_start(pin_name, f_desired, 0, RESOLUTION_8B_COMPARE_FORMAT);
  }

#endif

#endif // HAL_STM32
