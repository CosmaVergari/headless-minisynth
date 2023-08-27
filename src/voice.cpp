//
// voice.cpp
//
// MiniSynth Pi - A virtual analogue synthesizer for Raspberry Pi
// Copyright (C) 2017-2020  R. Stange <rsta2@o2online.de>
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
#include "voice.h"
#include <assert.h>



CVoice::CVoice (void)
:	m_VCO (&m_LFO_VCO),
	m_VCO2 (&m_LFO_VCO),
	m_VCO_Mixer (&m_VCO, &m_VCO2),
	m_VCF (&m_VCO_Mixer, &m_LFO_VCF, &m_EG_VCF),
	m_VCA (&m_VCF, &m_LFO_VCA, &m_EG_VCA),
	m_ucKeyNumber (KEY_NUMBER_NONE)
{
}

CVoice::~CVoice (void)
{
}

void CVoice::SetPatch (CPatch *pPatch)
{
	assert (pPatch != 0);

	// VCO
	m_LFO_VCO.SetWaveform ((TWaveform) pPatch->GetParameter (LFOVCOWaveform));
	m_LFO_VCO.SetFrequency (pPatch->GetParameter (LFOVCOFrequency));

	m_VCO.SetWaveform ((TWaveform) pPatch->GetParameter (VCOWaveform));
	m_VCO.SetModulationVolume (pPatch->GetParameter (VCOModulationVolume) / 100.0);

	m_VCO2.SetWaveform ((TWaveform) pPatch->GetParameter (VCOWaveform));
	m_VCO2.SetModulationVolume (pPatch->GetParameter (VCOModulationVolume) / 100.0);
	m_VCO2.SetDetune (pPatch->GetParameter (VCODetune) / 100.0 - 1.0);
	// TODO: Add SetOctave and octave in patch

	// VCF
	m_LFO_VCF.SetWaveform ((TWaveform) pPatch->GetParameter (LFOVCFWaveform));
	m_LFO_VCF.SetFrequency (pPatch->GetParameter (LFOVCFFrequency) / 10.0);

	m_VCF.SetCutoffFrequency (pPatch->GetParameter (VCFCutoffFrequency));
	m_VCF.SetResonance (pPatch->GetParameter (VCFResonance));

	m_EG_VCF.SetAttack (pPatch->GetParameter (EGVCFAttack));
	m_EG_VCF.SetDecay (pPatch->GetParameter (EGVCFDecay));
	m_EG_VCF.SetSustain (pPatch->GetParameter (EGVCFSustain) / 100.0);
	m_EG_VCF.SetRelease (pPatch->GetParameter (EGVCFRelease));

	m_VCF.SetModulationVolume (pPatch->GetParameter (VCFModulationVolume) / 100.0);

	// VCA
	m_LFO_VCA.SetWaveform ((TWaveform) pPatch->GetParameter (LFOVCAWaveform));
	m_LFO_VCA.SetFrequency (pPatch->GetParameter (LFOVCAFrequency) / 10.0);

	m_EG_VCA.SetAttack (pPatch->GetParameter (EGVCAAttack)); 
	m_EG_VCA.SetDecay (pPatch->GetParameter (EGVCADecay));
	m_EG_VCA.SetSustain (pPatch->GetParameter (EGVCASustain) / 100.0);
	m_EG_VCA.SetRelease (pPatch->GetParameter (EGVCARelease));

	m_VCA.SetModulationVolume (pPatch->GetParameter (VCAModulationVolume) / 100.0);
}

void CVoice::NoteOn (u8 ucKeyNumber, u8 ucVelocity)
{
	m_ucKeyNumber = ucKeyNumber;
	m_VCO.SetMIDINote (m_ucKeyNumber);
	m_VCO2.SetMIDINote (m_ucKeyNumber);

	assert (1 <= ucVelocity && ucVelocity <= 127);
	float fVelocityLevel = ucVelocity / 127.0;
	m_EG_VCF.NoteOn (fVelocityLevel);
	m_EG_VCA.NoteOn (fVelocityLevel);
}

void CVoice::NoteOff (void)
{
	m_EG_VCF.NoteOff ();
	m_EG_VCA.NoteOff ();
}

TVoiceState CVoice::GetState (void) const
{
	switch (m_EG_VCA.GetState ())
	{
	case EnvelopeStateIdle:
		return VoiceStateIdle;

	case EnvelopeStateAttack:
	case EnvelopeStateDecay:
	case EnvelopeStateSustain:
		return VoiceStateActive;

	case EnvelopeStateRelease:
		return VoiceStateRelease;

	default:
		assert (0);
		return VoiceStateActive;
	}
}

u8 CVoice::GetKeyNumber (void) const
{
	return m_EG_VCA.GetState () != EnvelopeStateIdle ? m_ucKeyNumber : KEY_NUMBER_NONE;
}

void CVoice::NextSample (void)
{
	// VCO
	m_LFO_VCO.NextSample ();
	m_VCO.NextSample ();
	m_VCO2.NextSample ();
	m_VCO_Mixer.NextSample ();

	// VCF
	m_LFO_VCF.NextSample ();
	m_EG_VCF.NextSample ();
	m_VCF.NextSample ();

	// VCA
	m_LFO_VCA.NextSample ();
	m_EG_VCA.NextSample ();
	m_VCA.NextSample ();
}

float CVoice::GetOutputLevel (void) const
{
	return m_VCA.GetOutputLevel ();
}
