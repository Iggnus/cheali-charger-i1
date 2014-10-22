
set(hardware imaxB6-clone)

set(SOURCE_FILES
    defaultCalibration.cpp
    HardwareConfig.h
)

CHEALI_CPU(nuvoton)
CHEALI_GENERIC_CHARGER(50W-generic)

CHEALI_GENERATE_ARM_EXEC()
