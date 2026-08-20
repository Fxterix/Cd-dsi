#---------------------------------------------------------------------------------
# Makefile estándar para Nintendo DS / DSi usando devkitARM
#---------------------------------------------------------------------------------

ifeq ($(strip $(DEVKITARM)),)
$(error "Por favor, configura DEVKITARM en tu entorno. (ej. export DEVKITARM=/opt/devkitpro/devkitARM)")
endif

include $(DEVKITARM)/ds_rules

#---------------------------------------------------------------------------------
# Configuración de carpetas y objetivo
#---------------------------------------------------------------------------------
TARGET		:=	$(notdir $(CURDIR))
BUILD		:=	build
SOURCES		:=	source
INCLUDES	:=	include
DATA		:=	data

#---------------------------------------------------------------------------------
# Opciones de compilación (Optimizadas para DSi/DS)
#---------------------------------------------------------------------------------
ARCH	:=	-marm -mthumb-interwork
CFLAGS	:=	-g -Wall -O2 \
		-march=armv5te -mtune=arm946e-s -fomit-frame-pointer \
		-ffast-math \
		$(ARCH)

CFLAGS	+=	$(INCLUDE) -DARM9
CXXFLAGS	:= $(CFLAGS) -fno-rtti -fno-exceptions

ASFLAGS	:=	-g $(ARCH)
LDFLAGS	=	-specs=ds_arm9.specs -g $(ARCH) -Wl,-Map,$(notdir $*.map)

#---------------------------------------------------------------------------------
# Librerías (libnds, etc.)
#---------------------------------------------------------------------------------
LIBS	:= -lnds9
LIBDIRS	:= $(LIBNDS)

#---------------------------------------------------------------------------------
# Reglas automáticas
#---------------------------------------------------------------------------------
ifneq ($(BUILD),$(notdir $(CURDIR)))
export OUTPUT	:=	$(CURDIR)/$(TARGET)
export VPATH	:=	$(foreach dir,$(SOURCES),$(CURDIR)/$(dir)) \
			$(foreach dir,$(DATA),$(CURDIR)/$(dir))
export DEPSDIR	:=	$(CURDIR)/$(BUILD)

CFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.c)))
CPPFILES	:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.cpp)))
SFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.s)))
BINFILES	:=	$(foreach dir,$(DATA),$(notdir $(wildcard $(dir)/*.*)))

export OFILES	:=	$(addsuffix .o,$(BINFILES)) \
			$(CPPFILES:.cpp=.o) $(CFILES:.c=.o) $(SFILES:.s=.o)

export INCLUDE	:=	$(foreach dir,$(INCLUDES),-I$(CURDIR)/$(dir)) \
			$(foreach dir,$(LIBDIRS),-I$(dir)/include) \
			-I$(CURDIR)/$(BUILD)

export LIBPATHS	:=	$(foreach dir,$(LIBDIRS),-L$(dir)/lib)

.PHONY: $(BUILD) clean all

#---------------------------------------------------------------------------------
all: $(BUILD)

$(BUILD):
	@[ -d $@ ] || mkdir -p $@
	@$(MAKE) --no-print-directory -C $(BUILD) -f $(CURDIR)/makefile.dsi

clean:
	@echo Limpiando el proyecto...
	@rm -rf $(BUILD) $(TARGET).elf $(TARGET).nds $(TARGET).dsi $(TARGET).map

else

DEPENDS	:=	$(OFILES:.o=.d)

#---------------------------------------------------------------------------------
# Regla principal de compilación para NDS/DSi
#---------------------------------------------------------------------------------
$(OUTPUT).nds	:	$(OUTPUT).elf
$(OUTPUT).elf	:	$(OFILES)

-include $(DEPENDS)

endif
