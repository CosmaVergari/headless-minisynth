//
// serialmididevice.h
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
#ifndef _serialcontroller_h
#define _serialcontroller_h

#include <circle/interrupt.h>
#include <circle/serial.h>
#include <circle/types.h>

class CMiniSynthesizer;

class CSerialController
{
public:
	CSerialController (CMiniSynthesizer *pSynthesizer, CInterruptSystem *pInterrupt);
	~CSerialController (void);

	boolean Initialize (void);

	void Process (void);

private:
	CSerialDevice m_Serial;
	unsigned m_nSerialState;
	u8 m_SerialMessage[3];
};

#endif
