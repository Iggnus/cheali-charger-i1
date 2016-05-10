#include "Hardware.h"
#include "SMPS_PID.h"
#include "IO.h"
#include "AnalogInputs.h"
#include "atomic.h"

#include "SerialLog.h"		//igntst


namespace SMPS_PID {
	
	//prefix "i" - variable is used in interrupts
	
	volatile uint16_t i_setpoint_adcIout;
	volatile uint16_t i_setpoint_adcVout;
	volatile uint16_t i_setpoint_Vout;
	//we have to use i_PID_CutOffVoltage, on some chargers (M0516) ADC can read up to 60V
//    volatile uint16_t i_cutOffVoltage;
	volatile long i_MV;
    volatile bool i_enable;

	void setPID_MV(long value);

	volatile uint16_t Vsoftstart;
//	uint8_t ignskip;
}

void SMPS_PID::update()
{

	if(!i_enable) return;

	AnalogInputs::ValueType adcVoutPlus  = AnalogInputs::getADCValue(AnalogInputs::Vout_plus_pin);
	AnalogInputs::ValueType adcVoutMinus = AnalogInputs::getADCValue(AnalogInputs::Vout_minus_pin);
//adcVoutPlus -= adcVoutMinus;


	//safety check
	//if Vout is too high disable PID
 	if(adcVoutPlus >= ANALOG_INPUTS_MAX_ADC_Vout_plus_pin) {
//SerialLog::printString("abnormal "); SerialLog::printUInt(adcVoutPlus); SerialLog::printNL(); //SerialLog::printUInt(AnalogInputs::getADCValue(AnalogInputs::Vout_plus_pin));
		powerOff();
		return;
	}
 
	long error_Iout = i_setpoint_adcIout;
	error_Iout -= AnalogInputs::getADCValue(AnalogInputs::Ismps);

	if(adcVoutPlus < i_setpoint_adcVout)
	{
		if(error_Iout > 0) i_MV += Vsoftstart;
		//else Vsoftstart = 0;
		else if(i_setpoint_adcVout) Vsoftstart = 0;
	}
	else if(i_setpoint_adcVout) Vsoftstart = 0;
	
	if(i_setpoint_adcVout) {
		if(i_setpoint_adcVout > adcVoutPlus && error_Iout > 0) i_MV += Vsoftstart;
		else Vsoftstart = 0;
	}


	if(Vsoftstart == 0) {
		error_Iout /= 64;
		i_MV += error_Iout;
		if(error_Iout >= 0) i_MV++;
		else i_MV--;
	}
	if(i_MV < 0) i_MV = 0;

	SMPS_PID::setPID_MV(i_MV);

//	if(--ignskip == 0){
//		SerialLog::printString("PID::upd "); SerialLog::printLong(error_Iout); SerialLog::printD();SerialLog::printLong(i_MV); SerialLog::printD(); SerialLog::printUInt(adcVoutPlus);  //igntst
//		SerialLog::printNL();  //igntst
//		ignskip == 200;
//	}
}

void SMPS_PID::initialize()
{
}

void SMPS_PID::powerOff()
{
	i_enable = false;
	hardware::setChargerOutput(false);
}

void SMPS_PID::powerOn()
{
	i_MV = 0;
	
	hardware::setChargerOutput(true);

	AnalogInputs::ValueType adcVoutPlus  = AnalogInputs::getADCValue(AnalogInputs::Vout_plus_pin);
	if(i_setpoint_adcVout > adcVoutPlus)
	{
		Vsoftstart = i_setpoint_adcVout - adcVoutPlus;
		Vsoftstart /= 256;
	//	Vsoftstart++;
		i_MV = AnalogInputs::calibrateValue(AnalogInputs::Vout_plus_pin, adcVoutPlus);
		
		i_MV *= 2; //34;		//ign	this is "ReverseCalibrate"
		i_MV /= 5;	//85;
		i_MV -= 19;
		if(i_MV < 0) i_MV = 0;

		i_MV = AnalogInputs::reverseCalibrateValue(AnalogInputs::IsmpsSet, i_MV);
	}
	else {
		Vsoftstart = 0;
		i_MV = i_setpoint_Vout;
	}
	
	i_enable = true;
	
}



//implementing
void SMPS_PID::setVoutPWM(uint16_t adcVout)
{
	long v = adcVout;
	v *= 2; //34;		//ign	this is "ReverseCalibrate"
	v /= 5;	//85;
	v -= 19;

	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		i_setpoint_adcVout = AnalogInputs::reverseCalibrateValue(AnalogInputs::Vout_plus_pin, adcVout);		//ign  voltage for feedback
		i_setpoint_Vout = AnalogInputs::reverseCalibrateValue(AnalogInputs::IsmpsSet, v);		//ign  Vend limit
	}
}


void SMPS_PID::setIoutPWM(uint16_t IoutPWM)
{
//	uint16_t Iout_ma = AnalogInputs::calibrateValue(AnalogInputs::IsmpsSet, IoutPWM);
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
//		i_setpoint_adcIout = AnalogInputs::reverseCalibrateValue(AnalogInputs::Ismps, Iout_ma);
		i_setpoint_adcIout = AnalogInputs::reverseCalibrateValue(AnalogInputs::Ismps, IoutPWM);
	}

//    hardware::setChargerValue(IoutPWM);
}

void SMPS_PID::setPID_MV(long value) {
	//if(value > i_setpoint_adcVout) i_MV = value = i_setpoint_adcVout;
	if(value > i_setpoint_Vout) i_MV = value = i_setpoint_Vout;
	
//	long v = value * 83;
//	v /= 152;
//	if(value) value = v + 22;
	hardware::setChargerValue((uint16_t)value);
	//if(value) v += 22;
	//hardware::setChargerValue((uint16_t)v);
}

