CONTIKI_PROJECT = solarPanel
all: $(CONTIKI_PROJECT)

EXTERNALDIRS += ../common

PROJECT_SOURCEFILES += helpers.c nodeID.c projSensors.c servoControl.c servo.c
PROJECT_SOURCEFILES += routing.c unicast_panel.c
CONTIKI_WITH_RIME = 1
CONTIKI = $(HOME)/contiki

CFLAGS += -DPROJECT_CONF_H=\"project-conf.h\"

include $(CONTIKI)/Makefile.include
