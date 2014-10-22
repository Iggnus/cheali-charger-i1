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

#include "Strategy.h"
#include "LcdPrint.h"
#include "Buzzer.h"
#include "memory.h"
#include "Monitor.h"
#include "PolarityCheck.h"
#include "AnalogInputs.h"


#include "ProgramData.h" 	 //ign
#include "TheveninChargeStrategy.h"		//ign
#include "TheveninDischargeStrategy.h"		//ign
#include "IO.h"			//ign

//#include "SerialLog.h"    //ign

namespace Strategy {
  uint8_t OnTheFly_;
  bool OnTheFly_blink;
    const VTable * strategy;
    bool exitImmediately;


    void chargingComplete() {
        Monitor::powerOff();
        lcdClear();
        //Screen::displayScreenProgramCompleted();
#ifndef ENABLE_T_INTERNAL
        hardware::setBatteryOutput(false); // ADD THIS LINE TO TURN OFF THE FAN
#endif
        Buzzer::soundProgramComplete();
#ifndef FREEZE_COMPLETED
		Screen::displayScreenProgramCompleted();
		waitButtonPressed();
#else
		AnalogInputs::powerOff();
		do {
			Screen::displayScreenProgramCompleted();
			if(waitButtonPressedLimTime()) break;
			Screen::displayScreenFirst();
			if(waitButtonPressedLimTime()) break;
		} while(true);
		AnalogInputs::powerOn();
#endif	
        Buzzer::soundOff();
		hardware::setBatteryOutput(true);  //ign
    }

    void chargingMonitorError() {
        Monitor::powerOff();
        lcdClear();
        Screen::displayMonitorError();
        Buzzer::soundError();
        waitButtonPressed();
        Buzzer::soundOff();
    }

    void strategyPowerOn() {
        void (*powerOn)() = pgm::read(&strategy->powerOn);
		OnTheFly_ = 0;
        powerOn();
    }
    void strategyPowerOff() {
        void (*powerOff)() = pgm::read(&strategy->powerOff);
        powerOff();
    }
    Strategy::statusType strategyDoStrategy() {
        Strategy::statusType (*doStrategy)() = pgm::read(&strategy->doStrategy);
        return doStrategy();
    }


    bool analizeStrategyStatus(Strategy::statusType status) {
        bool run = true;
        if(status == Strategy::ERROR) {
            strategyPowerOff();
//            AnalogInputs::powerOff();   //disconnect the battery (pin12 off)
            chargingMonitorError();
            run = false;
        }

        if(status == Strategy::COMPLETE) {
            strategyPowerOff();
            if(!exitImmediately)
                chargingComplete();
            run = false;
        }
        return run;
    }

    Strategy::statusType doStrategy(const Screen::ScreenType chargeScreens[])
    {
        uint8_t key;
        bool run = true;
        uint16_t newMesurmentData = 0;
        Strategy::statusType status = Strategy::RUNNING;
        strategyPowerOn();
        lcdClear();
        uint8_t screen_nr = 0;
        do {
            if(!PolarityCheck::runReversedPolarityInfo()) {
                Screen::display(pgm::read(&chargeScreens[screen_nr]));
            }

            //change displayed screen
            key =  Keyboard::getPressedWithSpeed();
			if(key) OnTheFly_blink = true;
			else OnTheFly_blink = !OnTheFly_blink;

			if(key == BUTTON_START) {
				if(Keyboard::getSpeed() || OnTheFly_ >= 2) OnTheFly_ = 0;
				else OnTheFly_ = 1;
			}
			else if(OnTheFly_ == 1 && key == BUTTON_NONE) OnTheFly_++;
			
			if(OnTheFly_ >= 2) {		// && pgm::read(&chargeScreens[screen_nr]) == Screen::ScreenFirst
				switch(pgm::read(&chargeScreens[screen_nr])) {
					case Screen::ScreenFirst:
						//if(Discharger::isWorking()) {
						if(!IO::digitalRead(DISCHARGE_DISABLE_PIN)) {
							if(key == BUTTON_DEC) {
								ProgramData::currentProgramData.changeId(-1);
								TheveninDischargeStrategy::setVI(1, ProgramData::currentProgramData.battery.Id);
							}
							if(key == BUTTON_INC) {
								ProgramData::currentProgramData.changeId(1);
								TheveninDischargeStrategy::setVI(1, ProgramData::currentProgramData.battery.Id);
							}
						}
						//if(SMPS::isWorking()) {
						if(!IO::digitalRead(SMPS_DISABLE_PIN)) {
							if(key == BUTTON_DEC) {
								ProgramData::currentProgramData.changeIc(-1);
								TheveninChargeStrategy::setVIB(1, ProgramData::currentProgramData.battery.Ic, true);
							}
							if(key == BUTTON_INC) {
								ProgramData::currentProgramData.changeIc(1);
								TheveninChargeStrategy::setVIB(1, ProgramData::currentProgramData.battery.Ic, true);
							}
						}
						break;
						
					case Screen::ScreenCIVlimits:
						//if(SMPS::isWorking() || Discharger::isWorking()) {
						if(!IO::digitalRead(SMPS_DISABLE_PIN) || !IO::digitalRead(DISCHARGE_DISABLE_PIN)) {
							if(key == BUTTON_DEC) {
								change0ToMaxSmart(Monitor::c_limit, -1, PROGRAM_DATA_MAX_CHARGE,0,10);
							}
							if(key == BUTTON_INC) {
								change0ToMaxSmart(Monitor::c_limit, 1, PROGRAM_DATA_MAX_CHARGE,0,10);
							}
						}
						break;

					case Screen::ScreenStartInfo:
							if(key == BUTTON_DEC) {
								ProgramData::currentProgramData.changeVoltage(-1);
							}
							if(key == BUTTON_INC) {
								ProgramData::currentProgramData.changeVoltage(1);
							}
						break;
#ifdef MANUAL_HISTORY
					case Screen::ScreenCycles:
							if(key == BUTTON_DEC) {
								OnTheFly_ = 3;
							}
							if(key == BUTTON_INC) {
								OnTheFly_ = 4;
							}
						break;
#endif
					default:
						OnTheFly_ = 0;
						break;
				}
			}
					
			else {
				if(key == BUTTON_INC && pgm::read(&chargeScreens[screen_nr+1]) != Screen::ScreenEnd) {
					if(status == Strategy::COMPLETE) {hardware::setBatteryOutput(true); }  // ADD THIS LINE TO TURN ON THE FAN

#ifdef ENABLE_SCREEN_ANIMATION
					Screen::displayAnimation();
#endif
                    screen_nr++;
                }
                if(key == BUTTON_DEC && screen_nr > 0) {
#ifdef ENABLE_SCREEN_ANIMATION
                    Screen::displayAnimation();
#endif
                    screen_nr--;
                }
            }

            if(run) {
                status = Monitor::run();
                run = analizeStrategyStatus(status);

                if(run && newMesurmentData != AnalogInputs::getFullMeasurementCount()) {
                    newMesurmentData = AnalogInputs::getFullMeasurementCount();
                    status = strategyDoStrategy();
                    run = analizeStrategyStatus(status);
                }
            }
            if(!run && exitImmediately)
                return status;
        } while(key != BUTTON_STOP || !Keyboard::getSpeed());

        strategyPowerOff();
        return status;
    }
} // namespace Strategy

