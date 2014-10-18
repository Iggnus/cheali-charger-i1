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
#include "Hardware.h"
#include "ProgramData.h"
#include "Screen.h"
#include "TheveninDischargeStrategy.h"
#include "Settings.h"
#include "memory.h"

//#include "SerialLog.h"    //ign

namespace TheveninDischargeStrategy {
    const Strategy::VTable vtable PROGMEM = {
        powerOn,
        powerOff,
        doStrategy
    };

    bool endOnTheveninMethodComplete_;
}



void TheveninDischargeStrategy::powerOff()
{
    Discharger::powerOff();
    Balancer::powerOff();
}


void TheveninDischargeStrategy::powerOn()
{
    Discharger::powerOn();
    Balancer::powerOn();
    //end on minimum Voltage reached or TheveninMethodComplete
    endOnTheveninMethodComplete_ = settings.dischargeAggressive_LiXX_;
    TheveninMethod::initialize(AnalogInputs::IdischargeValue);
}

void TheveninDischargeStrategy::setVI(AnalogInputs::ValueType v, AnalogInputs::ValueType i)
{
//SerialLog::printString("TDS::setVI "); SerialLog::printUInt(v); SerialLog::printD(); SerialLog::printUInt(i);  //ign
//SerialLog::printNL();  //ign
    SimpleDischargeStrategy::setVI(v,i);
//	TheveninMethod::setVIB(v, AnalogInputs::reverseCalibrateValue(AnalogInputs::IdischargeValue, i), false);		//ign_mA
    TheveninMethod::setVIB(v, i, false);		//ign_mA
    setMinI(i/10);
}
void TheveninDischargeStrategy::setMinI(AnalogInputs::ValueType i)
{
//	TheveninMethod::setMinI(AnalogInputs::reverseCalibrateValue(AnalogInputs::IdischargeValue, i));		//ign_mA
    TheveninMethod::setMinI(i);		//ign_mA
}

Strategy::statusType TheveninDischargeStrategy::doStrategy()
{
    bool stable;
    bool isEndVout = SimpleDischargeStrategy::isMinVout();
    uint16_t oldValue = Discharger::getValue();
    //uint16_t oldValue = AnalogInputs::calibrateValue(AnalogInputs::IdischargeValue, Discharger::getValue());		//ign_mA

    //when discharging near the end, the battery voltage is very unstable
    //but we need new discharge values at that point
    stable = AnalogInputs::isOutStable() || isEndVout;

    //test for charge complete
    bool end = isEndVout;
    if(endOnTheveninMethodComplete_) {
        end = TheveninMethod::isComlete(isEndVout, oldValue);
    }
    if(end) {
        Discharger::powerOff(Discharger::DISCHARGING_COMPLETE);
        return Strategy::COMPLETE;
    }

    if(stable) {
        uint16_t value = TheveninMethod::calculateNewValue(isEndVout, oldValue);
        if(value != oldValue) {
//			Discharger::setValue(value);		//ign_mA
            Discharger::setRealValue(value);		//ign_mA
        }
    }
    return Strategy::RUNNING;
}

