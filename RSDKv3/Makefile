#---------------------------------------------------------------------------------
.SUFFIXES:
#---------------------------------------------------------------------------------

ifeq ($(strip $(DEVKITARM)),)
$(error "Please set DEVKITARM in your environment. export DEVKITARM=<path>")
endif

include $(DEVKITARM)/ds_rules

TARGET		:= RetroEngineDS
BUILD		:= build
SOURCES		:= . 
DATA		:= data
INCLUDES	:= .

ARCH		:= -mthumb -mthumb-interwork

CFLAGS		:= -g -Wall -O2 \
			$(ARCH) \
			-DRETRO_PLATFORM=RETRO_DS \
			-DRETRO_USE_ORIGINAL_CODE=0 \
			$(INCLUDE)

CXXFLAGS	:= $(CFLAGS) -std=gnu++11 -fno-rtti -fno-exceptions

LIBS		:= -lnds9 -lnds7

CFILES		:= $(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.c)))
CPPFILES	:= $(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.cpp)))

export INCLUDE	:= $(foreach dir,$(INCLUDES),-I$(CURDIR)/$(dir)) \
		   $(foreach dir,$(SOURCES),-I$(CURDIR)/$(dir)) \
		   $(LIBNDS)/include

OFILES		:= $(CPPFILES:.cpp=.o) $(CFILES:.c=.o)

ifneq ($(BUILD),$(notdir $(CURDIR)))

export VPATH	:= $(foreach dir,$(SOURCES),$(CURDIR)/$(dir))
export OUTPUT	:= $(CURDIR)/$(TARGET)

.PHONY: all clean

all:
	@mkdir -p $(BUILD)
	@cd $(BUILD) && $(MAKE) -f $(CURDIR)/$@

clean:
	@rm -fr $(BUILD) $(TARGET).elf $(TARGET).nds $(TARGET).dsi

else

dependency	:= $(OFILES:.o=.d)

all: $(OUTPUT).dsi

$(OUTPUT).dsi: $(OUTPUT).nds
	@cp $(OUTPUT).nds $(OUTPUT).dsi

$(OUTPUT).nds: $(OFILES)

include $(DEVKITARM)/ds_arm9

endif
