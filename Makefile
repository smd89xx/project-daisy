export GDK ?= /opt/toolchains/mars/m68k-elf
MMLNK = $(CTRMML_DIR)/mdslink
MMLNK_ARGS = -o ./res/mdsseq.bin ./res/mdsbin.bin -h ./res/mdsseq.h
mdsdrv:
ifndef CTRMML_DIR
	$(error "Can not find MDSDRV music linker! Please set the CTRMML_DIR environment variable to its path.")
else
	$(MMLNK) $(MMLNK_ARGS) res/mus/*.mds
endif
include $(GDK)/makefile.gen