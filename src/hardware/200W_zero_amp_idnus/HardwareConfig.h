/*
    cheali-charger - open source firmware for a variety of LiPo chargers
    Copyright (C) 2013  Paweł Stawicki. All right reserved.

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
#ifndef HARDWARE_CONFIG_H_
#define HARDWARE_CONFIG_H_

#include "HardwareConfigGeneric.h"

#define MAX_CHARGE_V            ANALOG_VOLT(26.100) //25.3V
#define MAX_CHARGE_I            ANALOG_AMP(10.000)  //10A
#define MAX_CHARGE_P            ANALOG_WATT(200.000)  //200W

#define MAX_DISCHARGE_P         ANALOG_WATT(25.000)   //25W
#define MAX_DISCHARGE_I         ANALOG_AMP(5.000)   //5A


#define CALIBRATION_SPEED           1


#define CALIBRATION_CHARGE_POINT0_mA    50
#define CALIBRATION_CHARGE_POINT1_mA    1000
#define CALIBRATION_DISCHARGE_POINT0_mA 50
#define CALIBRATION_DISCHARGE_POINT1_mA 300


#define MANUAL_HISTORY

#define FREEZE_COMPLETED

//Do not work with batteries which use dynamic current mode at chargers without this feature!
#define DYNAMIC_MAX_CURRENT

//prevent power supply (>50W chargers)
#define ENABLE_SMOOTHCURRENT ANALOG_AMP(1.000)

//screenanimation
#define ENABLE_SCREENANIMATION

//knightrider effect is ADC working
//#define ENABLE_KNIGHTRIDEREFFECT

//use RAMCG battery icons for hitachi HD44780
#define ENABLE_RAM_CG

//audio mute function
#define ENABLE_MUTEAUDIO

//extended battery info (charge time limit)
#define ENABLE_TIME_LIMIT

//alternative battery name
//#define ENABLE_ALTERNATIVE_BATTERY_NAME

//enable Ic and Id from 0
#define ENABLE_ZERO_AMP


//#define ENABLE_CYCLING



#endif /* HARDWARE_CONFIG_H_ */

