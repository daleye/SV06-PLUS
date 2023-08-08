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

#include "../../inc/MarlinConfig.h"

#if ENABLED(LCD_SET_PROGRESS_MANUALLY)

#include "../gcode.h"
#include "../../lcd/marlinui.h"
#include "../../sd/cardreader.h"

#if ENABLED(DWIN_CREALITY_LCD_ENHANCED)
  #include "../../lcd/e3v2/enhanced/dwin.h"
#elif ENABLED(RTS_AVAILABLE)
  unsigned char percentComplete = 0;
  uint16_t timeRemaining = 0;
  #include "../../lcd/sv06p/LCD_RTS.h"
#endif


/**
 * M73: Set percentage complete (for display on LCD)
 *
 * Example:
 *   M73 P25 ; Set progress to 25%
 */
void GcodeSuite::M73() {

  #if ENABLED(DWIN_CREALITY_LCD_ENHANCED)

    DWIN_Progress_Update();

  #else

    if (parser.seenval('P'))
      ui.set_progress((PROGRESS_SCALE) > 1
        ? parser.value_float() * (PROGRESS_SCALE)
        : parser.value_byte());
      #if ENABLED(RTS_AVAILABLE)
        percentComplete = parser.value_byte();
        rtscheck.RTS_SndData((unsigned char)percentComplete, PRINT_PROCESS_ICON_VP);
        rtscheck.RTS_SndData((unsigned char)percentComplete, PRINT_PROCESS_VP);
        if (percentComplete == 100)
        {
          rtscheck.RTS_SndData(0, PRINT_TIME_HOUR_VP);
          rtscheck.RTS_SndData(0, PRINT_TIME_MIN_VP);
          rtscheck.RTS_SndData(0, PRINT_SURPLUS_TIME_HOUR_VP);
          rtscheck.RTS_SndData(0, PRINT_SURPLUS_TIME_MIN_VP);
          rtscheck.RTS_SDcard_Stop();
          PrintFlag = 0;
          Update_Time_Value = 0;
        }
      #endif

    #if ENABLED(USE_M73_REMAINING_TIME)
      if (parser.seenval('R')) ui.set_remaining_time(parser.value_ulong());
      #if ENABLED(RTS_AVAILABLE)
        timeRemaining = parser.value_ulong();
        rtscheck.RTS_SndData((timeRemaining / 60), PRINT_SURPLUS_TIME_HOUR_VP);
        rtscheck.RTS_SndData((timeRemaining % 60), PRINT_SURPLUS_TIME_MIN_VP);
      #endif
    #endif

  #endif
}

#endif // LCD_SET_PROGRESS_MANUALLY
