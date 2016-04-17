Welcome to cheali-charger!

Fork of Pawel's firmware [Branch:powerSupply at 01.11.2015] https://github.com/stawel/cheali-charger/tree/powerSupply

___

proof of concept - not for every day usage


	 THIS FIRMWARE IS FOR HW-MODED CHARGERS ONLY!
	______________________________________________


For 150W+ chargers with hardware voltage feedback

**Calibrate charge current BEFORE modification!**

----

How to modify

![Thunder T610](https://raw.githubusercontent.com/Iggnus/cheali-charger-i1/HW_moded_powerSupply/Docs/T610.jpg)
![Modification](https://raw.githubusercontent.com/Iggnus/cheali-charger-i1/HW_moded_powerSupply/Docs/T610_VFmod.jpg)

- Gyuiri's schematics: [turnigy 2X400](https://drive.google.com/file/d/0B1RXXTatsA1cczlMR184LUVZSkE), [turnigy 2x200](https://drive.google.com/file/d/0B1RXXTatsA1cb1R5NHM3MEtsakE), [turnigy 8150](https://drive.google.com/file/d/0B1RXXTatsA1cbkM2dXFxTldjTUU)

Disconnect R127 from "AA" point and "ground" it
Connect 240k resistor between "BATT+" terminal and pin2 of LM2904 #5 2/1

240k connected in series with C11 speeds up transient response (step can be skipped)



--

last changes


17.04.2016
...


	---
	   



Any feedback is very welcome!  

fork discussion - https://groups.google.com/forum/#!topic/cheali-charger/EuhUAoyecso

http://groups.google.com/group/cheali-charger

