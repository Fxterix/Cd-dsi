# Makefile corregido para RSDKv3 en DSi / NDS

PLATFORM ?= dsi
BUILDDIR = build

export THEME = 0

SOURCES = .
INCLUDES = .

# Configuración básica para devkitARM
include $(DEVKITPRO)/devkitARM/base_tools

.PHONY: all clean

all: $(BUILDDIR)
	$(MAKE) -C $(BUILDDIR) -f ../Makefile.dsi

$(BUILDDIR):
	mkdir -p $(BUILDDIR)

clean:
	rm -rf $(BUILDDIR)
