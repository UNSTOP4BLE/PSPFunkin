TARGET = PSPFunkin
OBJS = src/main.o \
	   src/game.o \
	   src/error.o \
	   src/chartparser.o \
	   src/psp/animation.o \
	   src/psp/audio.o \
	   src/psp/wav.o \
	   src/psp/pad.o \
	   src/psp/font.o \
	   src/psp/glib2d.o 

LIBS = -ljsoncpp -lstdc++ -lpng -lz -lpspgum -lpspgu -lpsprtc -lm -lpspvram -lpspaudio

CFLAGS = -O2 -G0 -Wall -g -fno-builtin-printf
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti -std=gnu++17
ASFLAGS = $(CFLAGS)

# PSP JUNK
PSP_FW_VERSION = 500
PSP_LARGE_MEMORY = 1

EXTRA_TARGETS = EBOOT.PBP
PSP_EBOOT_TITLE = PSPFunkin
PSP_EBOOT_ICON = ICON0.png
PSP_EBOOT_PIC1 = PIC1.png


PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak 