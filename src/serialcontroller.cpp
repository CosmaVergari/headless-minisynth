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
#include <assert.h>

#define CONTROLLER_BAUD 115200		// Set here the Baud Rate of the controller!

CSerialController::CSerialController (CMiniSynthesizer *pSynthesizer, CInterruptSystem *pInterrupt)
:
#if RASPPI <= 3 && defined (USE_USB_FIQ)
	m_Serial (pInterrupt, FALSE),
#else
	m_Serial (pInterrupt, TRUE),
#endif
	m_nSerialState (0)
{
}

CSerialController::~CSerialController (void)
{
	m_nSerialState = 255;
}

boolean CSerialController::Initialize (void)
{
	return m_Serial.Initialize (CONTROLLER_BAUD);
}

void CSerialController::Process (void)
{
	// Read serial MIDI data
	u8 Buffer[100];
	int nResult = m_Serial.Read (Buffer, sizeof Buffer);
	if (nResult <= 0)
	{
		return;
	}

	// Process Serial messages -> TODO
	for (int i = 0; i < nResult; i++)
	{
		u8 uchData = Buffer[i];
		if (uchData == 'c') {
			m_Serial.Write("ciao", 4);
		}

		// TODO: implement FSM accorind to serial_protocol spec
		switch (m_nSerialState)
		{
		case 0:
		case 1:
		case 2:

		default:
			//assert (0);
			break;
		}
	}
}
