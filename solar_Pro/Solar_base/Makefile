CONTIKI_PROJECT = base
all: $(CONTIKI_PROJECT)

EXTERNALDIRS += ../common

PROJECT_SOURCEFILES += helpers.c nodeID.c projSensors.c servoControl.c servo.c anemometer.c routing.c broadcast_common.c unicast_common.c
PROJECT_SOURCEFILES +=  unicast_local.c uart_local.c
CONTIKI_WITH_RIME = 1
CONTIKI = $(HOME)/contiki

CFLAGS += -DPROJECT_CONF_H=\"project-conf.h\"

include $(CONTIKI)/Makefile.include
