

set(CURRENT_PATH ${CMAKE_SOURCE_DIR}/src/atmega32/50W-generic)

set(GENERIC_SOURCE
    imaxB6.cpp
    imaxB6.h
    imaxB6-pins.h
    SMPS_PID.h
    SMPS_PID.cpp
    adc.cpp
    adc.h

    Hardware.h
    HardwareConfigGeneric.h
)

include_directories(${CURRENT_PATH})

CHEALI_FIND(GENERIC_SOURCE_FILES "${GENERIC_SOURCE}" "${CURRENT_PATH}")


