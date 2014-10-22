
set(SOURCE_FILES
    defaultCalibration.cpp
    HardwareConfig.h
)

CHEALI_HARDWARE(400W)
CHEALI_CPU(atmega32)
CHEALI_GENERIC_CHARGER(200W-generic)

CHEALI_GENERATE_AVR_EXEC()

