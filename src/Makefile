#
# Makefile
#

-include ../Config.mk

CIRCLEHOME ?= ../circle

OBJS	= main.o kernel.o minisynth.o mididevice.o \
	  midikeyboard.o pckeyboard.o serialcontroller.o voicemanager.o \
	  voice.o oscillator.o mixer.o filter.o amplifier.o envelopegenerator.o \
	  reverbmodule.o synthconfig.o patch.o parameter.o velocitycurve.o midiccmap.o

LIBS	= $(CIRCLEHOME)/addon/Properties/libproperties.a \
	  $(CIRCLEHOME)/addon/fatfs/libfatfs.a \
	  $(CIRCLEHOME)/addon/SDCard/libsdcard.a \
	  $(CIRCLEHOME)/lib/usb/libusb.a \
	  $(CIRCLEHOME)/lib/input/libinput.a \
	  $(CIRCLEHOME)/lib/fs/libfs.a \
	  $(CIRCLEHOME)/lib/sound/libsound.a \
	  $(CIRCLEHOME)/lib/libcircle.a

include $(CIRCLEHOME)/app/Rules.mk

-include $(DEPS)
