//
// oscillator.cpp
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
#include "oscillator.h"
#include "config.h"
#include "math.h"
#include <assert.h>

#define SINE_POINTS	360

float COscillator::s_SineTable[SINE_POINTS] =
{
0.00000000, 0.01745241, 0.03489950, 0.05233596, 0.06975647, 0.08715574, 0.10452846, 0.12186934,
0.13917310, 0.15643447, 0.17364818, 0.19080900, 0.20791169, 0.22495105, 0.24192190, 0.25881905,
0.27563736, 0.29237170, 0.30901699, 0.32556815, 0.34202014, 0.35836795, 0.37460659, 0.39073113,
0.40673664, 0.42261826, 0.43837115, 0.45399050, 0.46947156, 0.48480962, 0.50000000, 0.51503807,
0.52991926, 0.54463904, 0.55919290, 0.57357644, 0.58778525, 0.60181502, 0.61566148, 0.62932039,
0.64278761, 0.65605903, 0.66913061, 0.68199836, 0.69465837, 0.70710678, 0.71933980, 0.73135370,
0.74314483, 0.75470958, 0.76604444, 0.77714596, 0.78801075, 0.79863551, 0.80901699, 0.81915204,
0.82903757, 0.83867057, 0.84804810, 0.85716730, 0.86602540, 0.87461971, 0.88294759, 0.89100652,
0.89879405, 0.90630779, 0.91354546, 0.92050485, 0.92718385, 0.93358043, 0.93969262, 0.94551858,
0.95105652, 0.95630476, 0.96126170, 0.96592583, 0.97029573, 0.97437006, 0.97814760, 0.98162718,
0.98480775, 0.98768834, 0.99026807, 0.99254615, 0.99452190, 0.99619470, 0.99756405, 0.99862953,
0.99939083, 0.99984770, 1.00000000, 0.99984770, 0.99939083, 0.99862953, 0.99756405, 0.99619470,
0.99452190, 0.99254615, 0.99026807, 0.98768834, 0.98480775, 0.98162718, 0.97814760, 0.97437006,
0.97029573, 0.96592583, 0.96126170, 0.95630476, 0.95105652, 0.94551858, 0.93969262, 0.93358043,
0.92718385, 0.92050485, 0.91354546, 0.90630779, 0.89879405, 0.89100652, 0.88294759, 0.87461971,
0.86602540, 0.85716730, 0.84804810, 0.83867057, 0.82903757, 0.81915204, 0.80901699, 0.79863551,
0.78801075, 0.77714596, 0.76604444, 0.75470958, 0.74314483, 0.73135370, 0.71933980, 0.70710678,
0.69465837, 0.68199836, 0.66913061, 0.65605903, 0.64278761, 0.62932039, 0.61566148, 0.60181502,
0.58778525, 0.57357644, 0.55919290, 0.54463904, 0.52991926, 0.51503807, 0.50000000, 0.48480962,
0.46947156, 0.45399050, 0.43837115, 0.42261826, 0.40673664, 0.39073113, 0.37460659, 0.35836795,
0.34202014, 0.32556815, 0.30901699, 0.29237170, 0.27563736, 0.25881905, 0.24192190, 0.22495105,
0.20791169, 0.19080900, 0.17364818, 0.15643447, 0.13917310, 0.12186934, 0.10452846, 0.08715574,
0.06975647, 0.05233596, 0.03489950, 0.01745241, 0.00000000, -0.01745241, -0.03489950, -0.05233596,
-0.06975647, -0.08715574, -0.10452846, -0.12186934, -0.13917310, -0.15643447, -0.17364818, -0.19080900,
-0.20791169, -0.22495105, -0.24192190, -0.25881905, -0.27563736, -0.29237170, -0.30901699, -0.32556815,
-0.34202014, -0.35836795, -0.37460659, -0.39073113, -0.40673664, -0.42261826, -0.43837115, -0.45399050,
-0.46947156, -0.48480962, -0.50000000, -0.51503807, -0.52991926, -0.54463904, -0.55919290, -0.57357644,
-0.58778525, -0.60181502, -0.61566148, -0.62932039, -0.64278761, -0.65605903, -0.66913061, -0.68199836,
-0.69465837, -0.70710678, -0.71933980, -0.73135370, -0.74314483, -0.75470958, -0.76604444, -0.77714596,
-0.78801075, -0.79863551, -0.80901699, -0.81915204, -0.82903757, -0.83867057, -0.84804810, -0.85716730,
-0.86602540, -0.87461971, -0.88294759, -0.89100652, -0.89879405, -0.90630779, -0.91354546, -0.92050485,
-0.92718385, -0.93358043, -0.93969262, -0.94551858, -0.95105652, -0.95630476, -0.96126170, -0.96592583,
-0.97029573, -0.97437006, -0.97814760, -0.98162718, -0.98480775, -0.98768834, -0.99026807, -0.99254615,
-0.99452190, -0.99619470, -0.99756405, -0.99862953, -0.99939083, -0.99984770, -1.00000000, -0.99984770,
-0.99939083, -0.99862953, -0.99756405, -0.99619470, -0.99452190, -0.99254615, -0.99026807, -0.98768834,
-0.98480775, -0.98162718, -0.97814760, -0.97437006, -0.97029573, -0.96592583, -0.96126170, -0.95630476,
-0.95105652, -0.94551858, -0.93969262, -0.93358043, -0.92718385, -0.92050485, -0.91354546, -0.90630779,
-0.89879405, -0.89100652, -0.88294759, -0.87461971, -0.86602540, -0.85716730, -0.84804810, -0.83867057,
-0.82903757, -0.81915204, -0.80901699, -0.79863551, -0.78801075, -0.77714596, -0.76604444, -0.75470958,
-0.74314483, -0.73135370, -0.71933980, -0.70710678, -0.69465837, -0.68199836, -0.66913061, -0.65605903,
-0.64278761, -0.62932039, -0.61566148, -0.60181502, -0.58778525, -0.57357644, -0.55919290, -0.54463904,
-0.52991926, -0.51503807, -0.50000000, -0.48480962, -0.46947156, -0.45399050, -0.43837115, -0.42261826,
-0.40673664, -0.39073113, -0.37460659, -0.35836795, -0.34202014, -0.32556815, -0.30901699, -0.29237170,
-0.27563736, -0.25881905, -0.24192190, -0.22495105, -0.20791169, -0.19080900, -0.17364818, -0.15643447,
-0.13917310, -0.12186934, -0.10452846, -0.08715574, -0.06975647, -0.05233596, -0.03489950, -0.01745241
};


// See: http://www.deimos.ca/notefreqs/
static const float KeyFrequency[/* MIDI key number */] =
{
	8.17580, 8.66196, 9.17702, 9.72272, 10.3009, 10.9134, 11.5623, 12.2499, 12.9783, 13.7500,
	14.5676, 15.4339, 16.3516, 17.3239, 18.3540, 19.4454, 20.6017, 21.8268, 23.1247, 24.4997,
	25.9565, 27.5000, 29.1352, 30.8677, 32.7032, 34.6478, 36.7081, 38.8909, 41.2034, 43.6535,
	46.2493, 48.9994, 51.9131, 55.0000, 58.2705, 61.7354, 65.4064, 69.2957, 73.4162, 77.7817,
	82.4069, 87.3071, 92.4986, 97.9989, 103.826, 110.000, 116.541, 123.471, 130.813, 138.591,
	146.832, 155.563, 164.814, 174.614, 184.997, 195.998, 207.652, 220.000, 233.082, 246.942,
	261.626, 277.183, 293.665, 311.127, 329.628, 349.228, 369.994, 391.995, 415.305, 440.000,
	466.164, 493.883, 523.251, 554.365, 587.330, 622.254, 659.255, 698.456, 739.989, 783.991,
	830.609, 880.000, 932.328, 987.767, 1046.50, 1108.73, 1174.66, 1244.51, 1318.51, 1396.91,
	1479.98, 1567.98, 1661.22, 1760.00, 1864.66, 1975.53, 2093.00, 2217.46, 2349.32, 2489.02,
	2637.02, 2793.83, 2959.96, 3135.96, 3322.44, 3520.00, 3729.31, 3951.07, 4186.01, 4434.92,
	4698.64, 4978.03, 5274.04, 5587.65, 5919.91, 6271.93, 6644.88, 7040.00, 7458.62, 7902.13,
	8372.02, 8869.84, 9397.27, 9956.06, 10548.1, 11175.3, 11839.8, 12543.9
};

COscillator::COscillator (CSynthModule *pModulator)
:	m_pModulator (pModulator),
	m_Waveform (WaveformSine),
	m_fFrequency (20.0),
	m_fMidFrequency (m_fFrequency),
	m_uMIDINote(69),	// Middle A, nice
	m_fDetune (0.0),
	m_fOctave(0.0),
	m_fModulationVolume (0.0),
	m_nSampleCount (0),
	m_fOutputLevel (0.0),
	m_nRandSeed (1)
{
}

COscillator::~COscillator (void)
{
	m_pModulator = 0;
}

void COscillator::SetWaveform (TWaveform Waveform)
{
	assert (Waveform < WaveformUnknown);
	m_Waveform = Waveform;
}

void COscillator::SetFrequency (float fFrequency)
{
	assert (fFrequency > 0.0);
	m_fMidFrequency = fFrequency;
	m_fFrequency = exp2f (log2f (m_fMidFrequency) + m_fDetune + m_fOctave);
}

void COscillator::SetMIDINote (unsigned uMIDINote)
{
	if (uMIDINote < sizeof KeyFrequency / sizeof KeyFrequency[0])
	{
		assert (0 <= uMIDINote && uMIDINote <= 127);
		m_uMIDINote = uMIDINote;
		//Alt1: m_fMidFrequency = KeyFrequency[m_uMIDINote + (m_iOctave * 12)]; TODO: TEST!
		// m_fFrequency = exp2f (log2f (m_fMidFrequency) + m_fDetune);
		m_fMidFrequency = KeyFrequency[m_uMIDINote];
		m_fFrequency = exp2f (log2f (m_fMidFrequency) + m_fDetune + m_fOctave);
	}
}

// TODO: Maybe change the scaling of the detune (obiettivo: detune di 7 semitoni, fatto!)
void COscillator::SetDetune (float fDetune)
{
	assert (-1.0 <= fDetune && fDetune <= 1.0);
	m_fDetune = fDetune / 2.0; 
	m_fFrequency = exp2f (log2f (m_fMidFrequency) + m_fDetune + m_fOctave);
}

// TODO: Aggiungere supporto ottava
// TODO: non so se lasciare l'ottava come un valore relativo o no
void COscillator::SetOctave (int iOctave)
{
	assert(-3 <= iOctave && iOctave <= 2);
	m_fOctave = static_cast<float>(iOctave);
	m_fFrequency = exp2f (log2f (m_fMidFrequency) + m_fDetune + m_fOctave);
}

void COscillator::SetModulationVolume (float fVolume)
{
	assert (0.0 <= fVolume && fVolume <= 1.0);
	m_fModulationVolume = fVolume;
}

void COscillator::NextSample (void)
{
	float fFrequency = m_fFrequency;
	if (m_pModulator != 0)
	{
		fFrequency += m_pModulator->GetOutputLevel () * m_fModulationVolume * 20.0;
		if (fFrequency <= 0.0)
		{
			return;
		}
	}

	unsigned nPeriod = SAMPLE_RATE / fFrequency + 0.5;
	if (++m_nSampleCount >= nPeriod)
	{
		m_nSampleCount = 0;
	}

	switch (m_Waveform)
	{
	case WaveformSine:
		m_fOutputLevel = s_SineTable[m_nSampleCount * SINE_POINTS / nPeriod];
		break;

	case WaveformSquare:
		m_fOutputLevel = m_nSampleCount*2 < nPeriod ? 1.0 : -1.0;
		break;

	case WaveformSawtooth:
		m_fOutputLevel = -1.0 + (2.0 * m_nSampleCount) / nPeriod;
		break;

	case WaveformTriangle:
		m_fOutputLevel =   m_nSampleCount*2 < nPeriod
				 ? -1.0 + (2.0 * m_nSampleCount*2) / nPeriod
				 : 1.0 - (2.0 * (m_nSampleCount*2-nPeriod)) / nPeriod;
		break;

	case WaveformPulse12:
	case WaveformPulse25: {
		float fPulseWidth = m_Waveform == WaveformPulse12 ? 0.125 : 0.25;
		m_fOutputLevel = m_nSampleCount < nPeriod*fPulseWidth ? 1.0 : -1.0;
		} break;

	case WaveformWhiteNoise:
		m_fOutputLevel = rand_r (&m_nRandSeed) * (2.0 / RAND_MAX) - 1.0;
		break;

	default:
		assert (0);
		break;
	}
}

float COscillator::GetOutputLevel (void) const
{
	return m_fOutputLevel;
}
