#
# Makefile for Mobile Process 
# by luotang 2017.6.1
#

ROOTDIR = ../..
#include $(ROOTDIR)/Rules.make
CC		= arm-linux-gcc
BIN		= bin
TARGET	= $(BIN)/mobile

SRCS 	= $(wildcard src/*.c)
OBJS	= $(SRCS:%.c=$(BIN)/%.o)

IFLAGS 		= -I./inc 
CFLAGS 		= #-Wall -W -O2  -Wswitch-default -Wpointer-arith -g #-Wno-unused -g
#DFLAGS 		= -D_GNU_SOURCE 
LD_FLAGS 	= -lpthread 

GFLAG 		= $(CFLAGS) $(IFLAGS)# $(DFLAGS) 

.PHONY: all clean

all : $(TARGET)

$(TARGET): $(OBJS)
	@$(CC) -o $@ $^ $(LD_FLAGS) 
	@date 
	
#$(STRIP) $@
#$(STRIP) -x -R .note -R .comment $@

$(BIN)/%.o:%.c
	@mkdir -p $(BIN)
	@mkdir -p $(dir $@)
	@$(CC) $(GFLAG) -c $^ -o $@
	@echo $@
	@echo [CC] $^
clean :
	rm -rf $(BIN)/*