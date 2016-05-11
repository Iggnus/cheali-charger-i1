/*
    cheali-charger - open source firmware for a variety of LiPo chargers
    Copyright (C) 2013  Pawel Stawicki. All right reserved.

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
#include "memory.h"
#include "Screen.h"
#include "LcdPrint.h"
#include "ProgramData.h"
#include "TheveninMethod.h"
#include "Settings.h"
#include "Hardware.h"
#include "Program.h"
#include "DelayStrategy.h"
#include "Version.h"
#include "ProgramDCcycle.h"
#include "Monitor.h"
#include "PolarityCheck.h"
#include "ScreenMethods.h"
#include "Balancer.h"

#include "IO.h"    //ign
#include "StaticEditMenu.h"
#include "TheveninDischargeStrategy.h"
//#include "SerialLog.h"    //ign

using namespace ProgramData;
using namespace ProgramDataMenu;
namespace Screen { namespace Methods {

    void printCharge() {
        lcdPrintCharge(AnalogInputs::getRealValue(AnalogInputs::Cout), 8);
        lcdPrintSpace1();
    }

    void printCharAndTime() {
        char c = 'N';
        if(SMPS::isPowerOn()) {
            c = 'C';
        } else if(Discharger::isPowerOn()) {
            c = 'D';
            if(SMPS::isPowerOn()) c = 'E';
        } else if(::Balancer::isWorking()) {
            c = 'B';
        }

        if(DelayStrategy::isDelay()) {
            c = 'W';
        }
        lcdPrintChar(c);
        lcdPrintSpace1();
        lcdPrintTime(Monitor::getTimeSec());
        lcdPrintSpace1();
    }

    void printDeltaV() {
        int16_t x = AnalogInputs::getRealValue(AnalogInputs::deltaVout);
        lcdPrintSigned(x, 5);
        lcdPrintChar('m');
        lcdPrintChar('V');
        lcdPrintSpaces();

    }
    void printDeltaT() {
        if(ProgramData::battery.enable_externT) {
            int16_t x = AnalogInputs::getRealValue(AnalogInputs::deltaTextern);
            lcdPrintSigned(x*10, 5);
            lcdPrintChar('m');
            lcdPrintChar('C');
        } else {
            lcdPrint_P(PSTR("no dT/t"));
        }
        lcdPrintSpaces();
    }


} }// namespace Screen::Methods

void Screen::Methods::displayFirstScreen()
{
    lcdSetCursor0_0();
   if(Screen::OnTheFly_ == 2) {      //ign
    if(!IO::digitalRead(DISCHARGE_DISABLE_PIN)) {
      if(Screen::OnTheFly_dir) {
        ProgramData::changeId(Screen::OnTheFly_dir);
//        Strategy::setVI(ProgramData::VDischarged, false);
        Strategy::setVI(ProgramData::VNominal, false);
      }
      if(Screen::OnTheFly_blink) lcdPrintCurrent(Strategy::maxI, 9);
      else lcdPrintSpaces(9);
    }
    else if(!IO::digitalRead(SMPS_DISABLE_PIN)) {
      if(Screen::OnTheFly_dir) {
        ProgramData::changeIc(Screen::OnTheFly_dir);
//        Strategy::setVI(ProgramData::VCharged, true);
        Strategy::setVI(ProgramData::VNominal, true);
      }
      if(Screen::OnTheFly_blink) lcdPrintCurrent(Strategy::maxI, 9);
      else lcdPrintSpaces(9);
    }
    else lcdPrintSpaces(9);
  }
  else printCharge();
    AnalogInputs::printRealValue(AnalogInputs::Iout,     7);
    lcdPrintSpaces();

    lcdSetCursor0_1();
    printCharAndTime();
    AnalogInputs::printRealValue(AnalogInputs::VoutBalancer,     7);
    lcdPrintSpaces();
}

void Screen::Methods::displayCIVlimits()
{
    lcdSetCursor0_0();

   if(Screen::OnTheFly_ == 2 && Screen::OnTheFly_dir) {      //ign
    if(!IO::digitalRead(SMPS_DISABLE_PIN) || !IO::digitalRead(DISCHARGE_DISABLE_PIN)) {
      changeMinToMaxSmart(&Monitor::c_limit, Screen::OnTheFly_dir, 10, ANALOG_MAX_CHARGE);
    }
  }

  if(Screen::OnTheFly_ == 2 && !Screen::OnTheFly_blink) {
    lcdPrintSpaces(8);
  }
  else {
//    lcdPrintCharge(ProgramData::getCapacityLimit(), 8);
    lcdPrintCharge(Monitor::c_limit, 8);
  }

    lcdPrintSpace1();
    lcdPrintCurrent(Strategy::maxI, 7);
    //ProgramData::currentProgramData.printIcString();
    lcdPrintSpaces();

    lcdSetCursor0_1();
    lcdPrint_P(PSTR("Limits: "));
    lcdPrintVoltage(Strategy::endV, 7);
    lcdPrintSpaces();
}

void Screen::Methods::displayTime()
{
    lcdSetCursor0_0();
#ifdef ENABLE_TIME_LIMIT
   if(Screen::OnTheFly_ == 2 && Screen::OnTheFly_dir) {      //ign
    if(!IO::digitalRead(SMPS_DISABLE_PIN) || !IO::digitalRead(DISCHARGE_DISABLE_PIN)) {
      ProgramData::changeTime(Screen::OnTheFly_dir);
    }
  }

    //lcdPrintSpace1();    //igntst
  if(Screen::OnTheFly_ == 2 && !Screen::OnTheFly_blink) {
    lcdPrintSpaces(8);
  }
  else {
    //ProgramData::currentProgramData.printTimeString();
    lcdPrintAnalog(ProgramData::battery.time, 8, AnalogInputs::TimeLimitMinutes);

  }
    lcdPrintSpaces(2);
#else
    lcdPrint_P(PSTR("time:     "));
#endif
    lcdPrintTime(Monitor::getTimeSec());
    lcdSetCursor0_1();
    lcdPrint_P(PSTR("b "));
    lcdPrintTime(Monitor::getTotalBalanceTimeSec());
    lcdPrintSpaces(2);
    lcdPrintTime(Monitor::getTotalChargeDischargeTimeSec());
}


void Screen::Methods::displayR()
{
  Screen::OnTheFly_ = 0;
    lcdSetCursor0_0();
    lcdPrint_P(PSTR("batt. R="));
    lcdPrintResistance(TheveninMethod::getReadableBattRth(), 8);
    lcdPrintSpaces();
    lcdSetCursor0_1();
    if(Monitor::isBalancePortConnected) {
        lcdPrint_P(PSTR("wires R="));
        lcdPrintResistance(TheveninMethod::getReadableWiresRth(),8);
    }
    lcdPrintSpaces();
}

void Screen::Methods::displayVinput()
{
  if(Screen::OnTheFly_ == 2) Screen::Methods::displayFlyMenu();
  else {
    lcdSetCursor0_0();
    lcdPrint_P(PSTR("Vinput="));
    AnalogInputs::printRealValue(AnalogInputs::Vin, 7);
    lcdPrintSpaces();
    lcdSetCursor0_1();
    lcdPrint_P(PSTR(" limit="));
    lcdPrintAnalog(settings.inputVoltageLow, 7, AnalogInputs::Voltage);
    lcdPrintSpaces();
  }
}

void Screen::Methods::displayVout()
{
  Screen::OnTheFly_ = 0;
    lcdSetCursor0_0();
    lcdPrint_P(PSTR("Vout ="));
    AnalogInputs::printRealValue(AnalogInputs::Vout, 7);
    lcdPrintSpaces();
    lcdSetCursor0_1();
    lcdPrint_P(PSTR("Vbal.="));
    AnalogInputs::printRealValue(AnalogInputs::Vbalancer, 7);
    lcdPrintSpaces();
}

void Screen::Methods::displayTemperature()
{
  Screen::OnTheFly_ = 0;
    lcdSetCursor0_0();
    lcdPrint_P(PSTR("Text="));
    if(ProgramData::battery.enable_externT)
        AnalogInputs::printRealValue(AnalogInputs::Textern,    5);
    else
        lcdPrint_P(PSTR("-"));
    lcdPrintSpaces();

    lcdSetCursor0_1();
#ifdef ENABLE_T_INTERNAL
    lcdPrint_P(PSTR("Tint="));
    AnalogInputs::printRealValue(AnalogInputs::Tintern,    5);
#endif
    lcdPrintSpaces();
}

void Screen::Methods::displayDeltaFirst()
{
  Screen::OnTheFly_ = 0;
    lcdSetCursor0_0();
    printCharge();
    printDeltaT();

    lcdSetCursor0_1();
    printCharAndTime();
    printDeltaV();
}


void Screen::Methods::displayDeltaVout()
{
  Screen::OnTheFly_ = 0;
    lcdSetCursor0_0();
    lcdPrint_P(PSTR("maxVout="));
    AnalogInputs::printRealValue(AnalogInputs::deltaVoutMax, 7);
    lcdPrintSpaces();

    lcdSetCursor0_1();
    lcdPrint_P(PSTR("delta V= "));
    printDeltaV();
}

void Screen::Methods::displayDeltaTextern()
{
  Screen::OnTheFly_ = 0;
    lcdSetCursor0_0();
    lcdPrint_P(PSTR("Text="));
    if(ProgramData::battery.enable_externT) {
        lcdPrintTemperature(AnalogInputs::getDeltaLastT(), 9);
    } else {
        lcdPrint_P(PSTR("N/A"));
    }
    lcdPrintSpaces();

    lcdSetCursor0_1();
    lcdPrint_P(PSTR("delta T= "));
    printDeltaT();
}

void Screen::Methods::displayEnergy()
{
    lcdSetCursor0_0();

#ifdef ENABLE_DISCHARGE_POWER
  if(IO::digitalRead(DISCHARGE_DISABLE_PIN)) Screen::OnTheFly_ = 0;
  if(Screen::OnTheFly_ == 2 && Screen::OnTheFly_dir) {      //ign
    //if(!IO::digitalRead(SMPS_DISABLE_PIN) || !IO::digitalRead(DISCHARGE_DISABLE_PIN)) {
    if(!IO::digitalRead(DISCHARGE_DISABLE_PIN)) {
      changeMinToMaxSmart(&Discharger::p_dis_limit, Screen::OnTheFly_dir, 1, MAX_DISCHARGE_P);
    }
  }
  if(Screen::OnTheFly_ == 2) {
    if(!Screen::OnTheFly_blink) {
      lcdPrintSpaces(8);
    }
    else {
      //AnalogInputs::printRealValue(AnalogInputs::Pout, 8);
      lcdPrintUnsigned(Discharger::p_dis_limit, 5);
      lcdPrint_P(PSTR("0mW"));
    }
    lcdPrintSpaces(1);
  }
  else AnalogInputs::printRealValue(AnalogInputs::Pout, 8);

#else
  Screen::OnTheFly_ = 0;
    AnalogInputs::printRealValue(AnalogInputs::Pout, 8);
#endif

    lcdPrintSpace1();
    AnalogInputs::printRealValue(AnalogInputs::Iout, 7);
    lcdPrintSpaces();
    lcdSetCursor0_1();
    AnalogInputs::printRealValue(AnalogInputs::Eout, 8);
    lcdPrintSpaces(2);

    uint8_t procent = Monitor::getChargeProcent();
    lcdPrintUnsigned(procent, 4);
    lcdPrint_P(PSTR("%"));
    lcdPrintSpaces();
}

/*
#define COND_ALWAYS         StaticEditMenu::Always
#define COND_NiXX           1
#define COND_Pb             2
#define COND_LiXX           4
#define COND_NiZn           8
#define COND_LiXX_NiZn      (COND_LiXX+COND_NiZn)
#define COND_LiXX_NiZn_Pb   (COND_LiXX+COND_NiZn+COND_Pb)
#define COND_NiXX_Pb        (COND_NiXX+COND_Pb)
#define COND_BATTERY        (COND_NiXX+COND_Pb+COND_LiXX+COND_NiZn)
 */
const cprintf::ArrayData batteryTypeData  PROGMEM = {batteryString, &battery.type};
const StaticEditMenu::StaticEditData editData[] PROGMEM = {
{string_adaptiveDis,  15,  {CP_TYPE_ON_OFF,0,&battery.enable_adaptiveDischarge},  {1, 0, 1}},
{string_Vc_per_cell,  14,  {CP_TYPE_V,0,&battery.Vc_per_cell},      {10,ANALOG_VOLT(0.0),ANALOG_VOLT(5.0)}},
{string_Vd_per_cell,  15,  {CP_TYPE_V,0,&battery.Vd_per_cell},      {10,ANALOG_VOLT(0.0),ANALOG_VOLT(5.0)}},
{NULL,                  StaticEditMenu::Last}
};

uint16_t getSelector() {
    STATIC_ASSERT(LAST_BATTERY_CLASS == 6);
    uint16_t result = 1<<14;
    if(battery.type != NoneBatteryType) result += 1 << getBatteryClass();
    return result;
}

void editCallback(StaticEditMenu * menu, uint16_t * adr) {
    if(adr == &ProgramData::battery.enable_adaptiveDischarge) {
    TheveninDischargeStrategy::endOnTheveninMethodComplete_ = battery.enable_adaptiveDischarge;
        //ProgramData::changedType();
    } else if(adr == &ProgramData::battery.Vc_per_cell) {
        if(!IO::digitalRead(SMPS_DISABLE_PIN)) Strategy::setVI(ProgramData::VCharged, true);
        //ProgramData::changedCapacity();
    } else if(adr == &ProgramData::battery.Vd_per_cell) {
        if(!IO::digitalRead(DISCHARGE_DISABLE_PIN)) Strategy::setVI(ProgramData::VDischarged, false);
        //ProgramData::changedIc();
    } else if(adr == &ProgramData::battery.Id) {
        //ProgramData::changedId();
    }
//    ProgramData::check();
//    menu->setSelector(getSelector());
}

StaticEditMenu menu(editData, editCallback);
//StaticEditMenu menu(editData);
int8_t item;

void Screen::Methods::displayFlyMenu()
{
        menu.setSelector(getSelector());
        item = menu.runSimple();
        if(item < 0) Screen::OnTheFly_ = 0;
    else
        {
  //        ProgramData::Battery undo(ProgramData::battery);
            if(!menu.runEdit()) {
  //      ProgramData::battery = undo;
            }
      else {
        Buzzer::soundSelect();
            }
//            settings.apply();
        }
//  Screen::OnTheFly_ = 0;
}



