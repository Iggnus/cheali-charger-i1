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
#include "Utils.h"

#include "ScreenPages.h"

#include "IO.h"		//ign
//#include "SerialLog.h"    //ign

namespace Screen {

    Blink blink;
	
uint8_t OnTheFly_;
int8_t OnTheFly_dir;
bool OnTheFly_blink;
bool OnTheFly_blink2;
bool screen_animation;

    uint8_t pageNr_;
    uint8_t keyboardButton;

    //see PAGE_PROGRAM
    //see PAGE_BATTERY
    STATIC_ASSERT_MSG(ProgramData::LAST_BATTERY_CLASS == 6 && Program::LAST_PROGRAM_TYPE == 9 + 2, "see ScreenPages.h");

    uint32_t getConditions() {
        uint32_t c = 0;
        if(Program::programState == Program::Info)
            c += PAGE_START_INFO;
        if(Monitor::isBalancePortConnected)
            c += PAGE_BALANCE_PORT;
        c += PAGE_BATTERY(ProgramData::getBatteryClass());
        c += PAGE_PROGRAM(Program::programType);
        return c;
    }

    VoidMethod getPage(uint8_t page) {
        uint8_t i = 0;
        Pages::PageInfo info;
        uint32_t condition = getConditions();
        bool ok;
        page++;
        do {
            info = pgm::read(&Pages::pageInfo[i]);
            ok = ((info.conditionEnable & condition) > 0) && ((info.conditionDisable & condition) == 0);
            if(ok) {
                page--;
            }
            i++;
        } while(page);
        return info.displayMethod;
    }

    void displayPage() {
        Screen::Cycle::storeCycleHistoryInfo();
        blink.incBlinkTime();

        getPage(pageNr_)();
    }

    void displayAnimation();

} // namespace Screen



void Screen::doStrategy()
{
    if(!PolarityCheck::runReversedPolarityInfo()) {
        Screen::displayPage();
    }
//            key =  Keyboard::getPressedWithSpeed();
	if(keyboardButton) {OnTheFly_blink = OnTheFly_blink2 = true;}
	else if(OnTheFly_blink2) OnTheFly_blink2 = false;
	else OnTheFly_blink = !OnTheFly_blink;

	if(keyboardButton == BUTTON_START) {
		if(Keyboard::getSpeed() > 1 || OnTheFly_ >= 2) OnTheFly_ = 0;
		else OnTheFly_ = 1;
	}
	else if(OnTheFly_ == 1 && keyboardButton == BUTTON_NONE) OnTheFly_++;

	if(keyboardButton == BUTTON_INC) OnTheFly_dir = 1;
	else if(keyboardButton == BUTTON_DEC) OnTheFly_dir = -1;
//	if(keyboardButton == BUTTON_INC) OnTheFly_dir = keyboardButton;
//	else if(keyboardButton == BUTTON_DEC) OnTheFly_dir = keyboardButton;
	else OnTheFly_dir = 0;			

	if(OnTheFly_ < 2) {
		if(keyboardButton == BUTTON_INC && getPage(pageNr_ + 1) != NULL ) {
//				if(status == Strategy::COMPLETE) {hardware::setBatteryOutput(true); }  // ADD THIS LINE TO TURN ON THE FAN

#ifdef ENABLE_SCREEN_ANIMATION
			Screen::displayAnimation();
#endif
			pageNr_++;
		}
		if(keyboardButton == BUTTON_DEC && pageNr_ > 0) {
#ifdef ENABLE_SCREEN_ANIMATION
			Screen::displayAnimation();
#endif
			pageNr_--;
		}
	}

}

void Screen::powerOn()
{
    Screen::blink.startBlinkOn(0);
    pageNr_ = 0;
    Screen::Cycle::resetCycleHistory();
}

void Screen::powerOff() {}


void Screen::initialize() {
#ifdef SCREEN_START_DELAY_MS
    Time::delay(SCREEN_START_DELAY_MS); //waiting common display charger for display relase
#endif
#ifdef ENABLE_LCD_RAM_CG
    lcdCreateCGRam();
#endif
}

void Screen::displayStrings(const char *s)
{
    lcdClear();
    lcdSetCursor0_0();
    lcdPrint_P(s);
}

void Screen::displayStrings(const char *s1, const char *s2)
{
    displayStrings(s1);
    lcdSetCursor0_1();
    lcdPrint_P(s2);
}


namespace Screen {
    void screenEnd(const char * firstLine) {
        lcdSetCursor0_0();
        lcdPrint_P(firstLine);
        lcdPrintTime(Monitor::getTimeSec());
        lcdSetCursor0_1();
        lcdPrint_P(Program::stopReason);
#ifdef DISCH_VOLT_ERR
//AnalogInputs::ValueType VMout_V = AnalogInputs::calibrateValue(AnalogInputs::Vout_plus_pin, Monitor::VMout);
//lcdPrintVoltage(VMout_V, 5);
if(Monitor::VMoutERR != 65535) lcdPrintVoltage(Monitor::VMoutERR, 5);
#endif
        lcdPrintSpaces();
    }
}

void Screen::displayScreenProgramCompleted()
{
    screenEnd(PSTR("complete: "));
}

void Screen::displayMonitorError()
{
    screenEnd(PSTR("error:    "));
}


void Screen::displayNotImplemented()
{
    displayStrings(PSTR("N/A"));
}


void Screen::runNotImplemented()
{
    displayNotImplemented();
    waitButtonPressed();
}

void Screen::displayScreenReversedPolarity()
{
    displayStrings(PSTR("REVERSE POLARITY"));
}

void Screen::displayAnimation()
{
	screen_animation = true;
    for (uint8_t i=0; i<16; i++) {
        lcdSetCursor(15-i,1);
        lcdPrintChar(255);
        lcdSetCursor(i,0);
        lcdPrintChar(255);
        Time::delayDoIdle(10);
    }
	Time::delayDoIdle(100);		//igntst
	screen_animation = false;
}

void Screen::displayCalibrationErrorScreen(uint8_t errNo)
{
    lcdClear();
    lcdSetCursor0_0();
    lcdPrint_P(PSTR("Cal.err.  F:"));
    lcdPrintUnsigned(errNo, 2);
    Time::delay(8000);
}

void Screen::runAskResetEeprom(uint8_t what)
{
    lcdClear();
    lcdSetCursor0_0();
    lcdPrint_P(PSTR("eeprom reset:"));
    lcdPrintUInt(what);
    lcdSetCursor0_1();
    lcdPrint_P(PSTR("            yes"));
    while (waitButtonPressed() != BUTTON_START);
}

void Screen::displayResettingEeprom()
{
    displayStrings(PSTR("resetting eeprom"));
}


void Screen::runResetEepromDone(uint8_t before, uint8_t after) {
    if(after != 0) {
        displayStrings(PSTR("eeprom reset\n"
                            "error: "));
        lcdPrintUInt(after);
    } else {
        //TODO
        //if(before )
        displayStrings(PSTR("please calibrate"));
        waitButtonPressed();
    }
}

void Screen::runCalibrationError(const char *s) {
    displayStrings(PSTR("calibation error"), s);
    waitButtonPressed();
}


void Screen::runWelcomeScreen() {
    Screen::displayStrings(PSTR( CHEALI_CHARGER_PROJECT_NAME_STRING "\n"
                                "v" CHEALI_CHARGER_VERSION_STRING));
    Time::delay(1000);
}

void Screen::runNeedForceBalance() {
    Screen::displayStrings(PSTR("NEED force bal.\n"
                                "set. --> YES"));
    Time::delay(2000);
}
