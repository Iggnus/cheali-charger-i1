
set(CORE_DIR ${CMAKE_SOURCE_DIR}/src/core/)

set(CORE_SOURCE
	AnalogInputs.cpp
	AnalogInputs.h
	AnalogInputsPrivate.h
	AnalogInputsTypes.h
	Balancer.cpp
	Balancer.h
	Blink.cpp
	Blink.h
	Buzzer.cpp
	Buzzer.h
	Calibrate.cpp
	Calibrate.h
	ChealiCharger2.cpp
	ChealiCharger2.h
	DelayStrategy.cpp
	DelayStrategy.h
	DeltaChargeStrategy.cpp
	DeltaChargeStrategy.h
	Discharger.cpp
	Discharger.h
	DummyLiquidCrystal.h
	EditMenu.cpp
	EditMenu.h
	EditName.cpp
	EditName.h
	Keyboard.cpp
	Keyboard.h
	LcdPrint.cpp
	LcdPrint.h
	LiquidCrystal.cpp
	LiquidCrystal.h
	MainMenu.h
	Menu.cpp
	Menu.h
	Monitor.cpp
	Monitor.h
	Options.cpp
	Options.h
	PolarityCheck.cpp
	PolarityCheck.h
	Program.cpp
	Program.h
	ProgramDCcycle.cpp
	ProgramDCcycle.h
	ProgramData.cpp
	ProgramData.h
	ProgramDataMenu.cpp
	ProgramDataMenu.h
	ProgramMenus.cpp
	ProgramMenus.h
	SMPS.cpp
	SMPS.h
	Screen.cpp
	Screen.h
	SerialLog.cpp
	SerialLog.h
	Settings.cpp
	Settings.h
	SettingsMenu.cpp
	SettingsMenu.h
	SimpleChargeStrategy.cpp
	SimpleChargeStrategy.h
	SimpleDischargeStrategy.cpp
	SimpleDischargeStrategy.h
	StackInfo.h
	StartInfoStrategy.cpp
	StartInfoStrategy.h
	StaticMenu.cpp
	StaticMenu.h
	StorageStrategy.cpp
	StorageStrategy.h
	Strategy.cpp
	Strategy.h
	Thevenin.cpp
	Thevenin.h
	TheveninChargeStrategy.cpp
	TheveninChargeStrategy.h
	TheveninDischargeStrategy.cpp
	TheveninDischargeStrategy.h
	TheveninMethod.cpp
	TheveninMethod.h
	Time.cpp
	Time.h
	Utils.cpp
	Utils.h
	eeprom.cpp
	eeprom.h
	standard.h
	strings.cpp
	strings_common.h
)

SET_SOURCE_FILES_PROPERTIES(${CORE_DIR}/strings.cpp PROPERTIES OBJECT_DEPENDS ${CORE_DIR}/standard.h)


include_directories(${CORE_DIR})

CHEALI_FIND("CORE_SOURCE_FILES" "${CORE_SOURCE}" "${CORE_DIR}" )

