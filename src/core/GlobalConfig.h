/*
    cheali-charger - open source firmware for a variety of LiPo chargers
    Copyright (C) 2014  Paweł Stawicki. All right reserved.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef GLOBAL_CONFIG_H_
#define GLOBAL_CONFIG_H_

#define LCD_LINES               2
#define LCD_COLUMNS             16

#define CALIBRATION_CHARGE_POINT0_mA    100
#define CALIBRATION_CHARGE_POINT1_mA    1000
#define CALIBRATION_DISCHARGE_POINT0_mA 100
#define CALIBRATION_DISCHARGE_POINT1_mA 300

#define ENABLE_SERIAL_LOG
#define ENABLE_TIME_LIMIT
#define ENABLE_LCD_RAM_CG
#define ENABLE_SCREEN_ANIMATION
//#define ENABLE_SCREEN_KNIGHTRIDEREFFECT

#define ENABLE_EEPROM_CRC
#define ENABLE_EEPROM_RESTORE_DEFAULT
#define ENABLE_SETTINGS_MENU_RESET

#define ENABLE_CALIBRATION
#define ENABLE_CALIBRATION_CHECK


#define FREEZE_COMPLETED
#define MANUAL_HISTORY
//#define ENABLE_AutoTime

#define ENABLE_PROGRAM_MENUS_LIXX_CYCLING
#define ENABLE_PROGRAM_MENUS_NIZN_CYCLING

/*
 * (experimental and dangerous)
 * maximum charge current will be determined
 * dynamically based on power supply voltage
 */
//#define ENABLE_DYNAMIC_MAX_POWER

#define STRINGS_HEADER "strings/standard.h"

#endif /* GLOBAL_CONFIG_H_ */
