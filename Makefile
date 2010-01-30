#NOTE: This file changes the fuse values of the chip to enable brown-out detection.
#fuse settings are hard-coded into the bottom lines; change them only with care.

PRG            = dialadong
PORT		   = /dev/ttyUSB0
PROGRAMMER     = stk500v1
OPTIMIZE       = -Os
LIBS           =
AVR = 328
MOG_DEBUG = 1

ifeq ($(AVR), 168)
	MCU_TARGET = atmega168
	AVRDUDE_TARGET = m168
	SIZE = 14336
else
	MCU_TARGET = atmega328p
	AVRDUDE_TARGET = m328p
	SIZE = 30720
endif

ifeq ($(MOG_DEBUG), 1)
	OBJ = main.o stack.o g2100.o network.o uip.o uip_arp.o timer.o dhcpc.o clock-arch.o psock.o tdtp.o serial.o
	DEFS = -DF_CPU=16000000L -DMOG_DEBUG
else
	OBJ = main.o stack.o g2100.o network.o uip.o uip_arp.o timer.o dhcpc.o clock-arch.o psock.o tdtp.o
	DEFS = -DF_CPU=16000000L
endif

# You should not have to change anything below here.

CC             = avr-gcc

# Override is only needed by avr-lib build system.

override CFLAGS        = -g -Wall -Wl,-static -Wl,--gc-sections -ffunction-sections -fdata-sections -Wl,-s $(OPTIMIZE) -mmcu=$(MCU_TARGET) $(DEFS)
override LDFLAGS       = -Wl,-Map,$(PRG).map

OBJCOPY        = avr-objcopy
OBJDUMP        = avr-objdump

all: $(PRG).elf lst text #eeprom
	@echo `ls -l $(PRG).bin |cut -f5 -d' '` bytes  of $(SIZE) total available

$(PRG).elf: $(OBJ)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

clean:
	rm -rf *.o $(PRG).elf *.eps *.png *.pdf *.bak *.hex *.bin *.srec
	rm -rf *.lst *.map $(EXTRA_CLEAN_FILES)

lst:  $(PRG).lst

%.lst: %.elf
	$(OBJDUMP) -h -S $< > $@

# Rules for building the .text rom images

text: hex bin srec

hex:  $(PRG).hex
bin:  $(PRG).bin
srec: $(PRG).srec

%.hex: %.elf
	$(OBJCOPY) -g -S -j .text -j .data -O ihex $< $@

%.srec: %.elf
	$(OBJCOPY) -g -S -j .text -j .data -O srec $< $@

%.bin: %.elf
	$(OBJCOPY) -g -S -j .text -j .data -O binary $< $@

# Rules for building the .eeprom rom images

eeprom: ehex ebin esrec


ehex:  $(PRG)_eeprom.hex
#ebin:  $(PRG)_eeprom.bin
esrec: $(PRG)_eeprom.srec

%_eeprom.hex: %.elf
	$(OBJCOPY) -j .eeprom --change-section-lma .eeprom=0 -O ihex $< $@

#%_eeprom.srec: %.elf
#	$(OBJCOPY) -j .eeprom --change-section-lma .eeprom=0 -O srec $< $@

%_eeprom.bin: %.elf
	$(OBJCOPY) -j .eeprom --change-section-lma .eeprom=0 -O binary $< $@

install: program
# command to program chip (invoked by running "make install")
program:
	stty -F $(PORT) hupcl
	avrdude -Cavrdude.conf -p $(AVRDUDE_TARGET) -c $(PROGRAMMER) -P $(PORT) \
         -b57600 -F  -Uflash:w:$(PRG).hex:i


#  Want to use 12 MHz low-power crystal oscillator now
# low fuse byte settings:  #1111 1111 = $FF
# bit 7: 1		(DO NOT div by 8)
# bit 6: 1		(clock output)
# bits 5,4: SUT1,0: 11
# bits 3,2,1,0: CKSEL:	1111  (for 8-16 mHz crystal oscillator)



#  lfuse:w:0xE2:m <-> #226 <-> 1110 0010
#  lower four bits: 0010 -> calibrated RC oscillator
#  Upper four bits: CKDiv8, CKout, SUT1, SUT0
#		This gives 8 MHz system clock, slow start up
#		Clock output on pin 14?



# Original fuse settings:
#	 -U lfuse:w:0x64:m  -U hfuse:w:0xDF:m	-U efuse:w:0xff:m
