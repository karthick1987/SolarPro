CONTIKI_PROJECT = solarPro
all: $(CONTIKI_PROJECT)

EXTERNALDIRS += ../common

PROJECT_SOURCEFILES += helpers.c
PROJECT_SOURCEFILES += nodeID.c

CONTIKI_WITH_RIME = 1
CONTIKI = $(HOME)/contiki

CFLAGS += -DPROJECT_CONF_H=\"project-conf.h\"

include $(CONTIKI)/Makefile.include
