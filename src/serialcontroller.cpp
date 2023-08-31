//
// serialmididevice.cpp
//
// MiniSynth Pi - A virtual analogue synthesizer for Raspberry Pi
// Copyright (C) 2017-2022  R. Stange <rsta2@o2online.de>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
#include "serialcontroller.h"
#include <circle/sysconfig.h>
#include <circle/logger.h>
#include <circle/string.h>
#include <assert.h>

#define CONTROLLER_BAUD 115200		// Set here the Baud Rate of the controller!

static const char FromSerialController[] = "serial_ctrl";

const u8 StartSequence = 0x80;
const u8 StopSequence = 0xC0;

CSerialController::CSerialController (CMiniSynthesizer *pSynthesizer, CInterruptSystem *pInterrupt)
:
#if RASPPI <= 3 && defined (USE_USB_FIQ)
	m_Serial (pInterrupt, FALSE),
#else
	m_Serial (pInterrupt, TRUE),
#endif
	m_nSerialState (START),
	m_uControlAddress(0),
	m_uControlValue(0)
{
}

CSerialController::~CSerialController (void)
{
	m_nSerialState = SERIAL_UNKNOWN;
}

boolean CSerialController::Initialize (void)
{
	return m_Serial.Initialize (CONTROLLER_BAUD);
}

void nibbleToHex(u8 nibble, char *string) {
	nibble &= 0b00001111;
	if (nibble < 10) {
		string[0] = '0' + nibble;
	} else {
		string[0] = 'A' + nibble - 10;
	}
}

void byteToHex(u8 byte, char *string) {
	nibbleToHex((byte & 0b11110000) >> 4, string); 
	nibbleToHex(byte & 0b00001111, string+1); 
}

void bufferToHex(u8 *Buffer, int bufLen, char *bufferHex) {
	for (int i = 0; i < bufLen; i++) {
		byteToHex(Buffer[i], bufferHex+(i*2));
	}
}

bool evaluateParity(u16 valueToCheck, u8 parityBit) {
	u8 parity, bit;
    for (bit=0, parity=0; bit < 16; bit++) {
        parity ^= (valueToCheck & (1<<bit)) ? 1 : 0;
    }
	parityBit &= 0x01;
    return parity == parityBit;
}


void CSerialController::Process (void)
{
	// Read serial data
	u8 Buffer[100];
	int nResult = m_Serial.Read (Buffer, sizeof Buffer);
	if (nResult <= 0)
	{
		return;
	}

	// For debugging: Print hex dump of serial
	//char bufferHex[201];
	//CLogger::Get ()->Write (FromSerialController, LogDebug, bufferHex);

	// Process Serial messages
	for (int i = 0; i < nResult; i++)
	{
		u8 uchData = Buffer[i];
		
		switch (m_nSerialState)
		{
		case START:
			if (uchData == StartSequence) {
				m_nSerialState = READ_ADDRESS;
				m_uControlAddress = 0;
				m_uControlValue = 0;
			} else {
				CLogger::Get ()->Write (FromSerialController, LogDebug, "error start");
			}
			break;
		case READ_ADDRESS:
			// If first bit is set (it is a control message) there was an error, restart
			if (uchData & (0b10000000)) {
				m_nSerialState = START;
				break;
			}
			m_uControlAddress = uchData;
			m_nSerialState = READ_VALUE1;
			break;
		case READ_VALUE1:
			if (uchData & 0b10000000) {
				m_nSerialState = START;
				break;
			}
			m_uControlValue = uchData << 7;
			m_nSerialState = READ_VALUE2;
			break;
		case READ_VALUE2:
			if (uchData & 0b10000000) {
				m_nSerialState = START;
				break;
			}
			m_uControlValue |= uchData;
			m_nSerialState = READ_STOP;
			break;
		case READ_STOP:
			// If the stop sequence is incorrect there was an error, restart
			if ((uchData & 0b11000000) == StopSequence) {
				if (evaluateParity(m_uControlValue ^ ((u16) m_uControlAddress), uchData & 0x01)) {
					CString buf;
					buf.Format("Reached STOP: Address %x Value %x", m_uControlAddress, m_uControlValue);
					CLogger::Get ()->Write (FromSerialController, LogDebug, buf);
				}
			}
			m_nSerialState = START;
			break;
		default:
			assert (0);
			break;
		}
	}
}
