TITLE_ID = PSVIBE001
TARGET = PSVibe
OBJS   = main.o

LIBS = -lvita2d -lScePgf_stub -lSceGxm_stub -lSceDisplay_stub -lSceCtrl_stub -lSceSysmodule_stub -lpng -lz -lm

PREFIX  = arm-vita-eabi
CC      = $(PREFIX)-gcc
CFLAGS  = -Wl,-q -Wall -O3
ASFLAGS = $(CFLAGS)

all: $(TARGET).vpk

$(TARGET).vpk: eboot.bin
	vita-mksfoex -s TITLE_ID=$(TITLE_ID) "$(TARGET)" param.sfo
	vita-pack-vpk -s param.sfo -b eboot.bin -a res/icon0.png=sce_sys/icon0.png \
	-a res/bg.png=sce_sys/livearea/contents/bg.png \
	-a res/startup.png=sce_sys/livearea/contents/startup.png \
	-a res/template.xml=sce_sys/livearea/contents/template.xml $@

eboot.bin: $(TARGET).velf
	vita-make-fself -s $< eboot.bin

$(TARGET).velf: $(TARGET).elf
	vita-elf-create $< $@

$(TARGET).elf: $(OBJS)
	$(CC) $(CFLAGS) $^ $(LIBS) -o $@

clean:
	@rm -rf *.velf *.elf *.vpk $(OBJS) param.sfo eboot.bin

vpksend: $(TARGET).vpk
	curl -T $(TARGET).vpk ftp://$(PSVITAIP):1337/ux0:/
	@echo "Sent."

send: eboot.bin
	curl -T eboot.bin ftp://$(PSVITAIP):1337/ux0:/app/$(TITLE_ID)/
	@echo "Sent."
