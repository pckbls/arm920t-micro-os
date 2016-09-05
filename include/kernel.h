#ifndef KERNEL_H_
#define KERNEL_H_

extern unsigned char _start_user, _end_user, _start_user_virt, _end_user_virt;
extern unsigned char _start_user_stack, _end_user_stack;
extern unsigned char _start_data_sections, _end_data_sections;

/* Systemdateien */
#include "config.h"
#include "cpu.h"
#include "exceptions.h"
#include "syscalls.h"
#include "process.h"
#include "threads.h"

/* Treiber */
#include "leds.h"
#include "serial.h"
#include "systemtimer.h"
#include "cp15.h"

/* Hilfsfunktionen */
#include "memory.h"
#include "ringbuffer.h"
#include "functions.h"

/* Userspace-Bibliothek */
#include "lib.h"

#define NULL 0

#define UNUSED(x) (void)(x)

#define BLOCK_1MB(x) ((x) >> 20)

#ifdef DEBUG_MESSAGES
	#define DEBUG(fmt, ...) serial_printf(fmt, ##__VA_ARGS__)
#else
	#define DEBUG(fmt, ...) 
#endif

#endif
