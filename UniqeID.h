#pragma once

namespace UniqeID
{
	void read_EE(uint8_t word, uint8_t *buf, uint8_t offset) {
		noInterrupts();
		FTFL_FCCOB0 = 0x41;             // Selects the READONCE command
		FTFL_FCCOB1 = word;             // read the given word of read once area

										// launch command and wait until complete
		FTFL_FSTAT = FTFL_FSTAT_CCIF;
		while (!(FTFL_FSTAT & FTFL_FSTAT_CCIF))
			;
		*(buf + offset + 0) = FTFL_FCCOB4;
		*(buf + offset + 1) = FTFL_FCCOB5;
		*(buf + offset + 2) = FTFL_FCCOB6;
		*(buf + offset + 3) = FTFL_FCCOB7;
		interrupts();
	}


	void read_myID(uint32_t *myID) {
		//read_EE(0xe, myID, 0); // should be 04 E9 E5 xx, this being PJRC's registered OUI
		read_EE(0xf, (uint8_t*)myID, 4); // xx xx xx xx, good for id
	}
}