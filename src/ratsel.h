#ifndef RATSEL_H
#define RATSEL_H

#define RATSEL_MAJOR 1
#define RATSEL_MINOR 0
#define RATSEL_PATCH 0

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <malloc.h>

#include "../vendor/hashmap/hashmap.h"

#define RTSL_UNUSED(_v) (void)(_v);
#define RTSL_SUCCESS 0
#define RTSL_FAILURE 1
#define RTSL_COMMAND_LINE_SIZE 512
#define RTSL_EXTENSION ".rtsl"

#define CLOG_WARN "[ WARNING ]: "
#define CLOG_INFO "[ INFO ]:    "
#define CLOG_ERR "[ ERROR ]:   "

#ifdef _DEBUG
#define d_clogf(_fmt, ...) printf(_fmt "\n", __VA_ARGS__);
#else
#define d_clogf(_fmt, ...)
#endif

#define clogf(_fmt, ...) printf(_fmt "\n", __VA_ARGS__);

typedef int (*RTSLFunction)(void *_app);

typedef struct
{
    struct hashmap *functions;
    char *command_line;

    char arg0[RTSL_COMMAND_LINE_SIZE];
    char arg1[RTSL_COMMAND_LINE_SIZE];
    char arg2[RTSL_COMMAND_LINE_SIZE];
} RTSLApp;

typedef struct
{
    char *name;
    RTSLFunction function;
} RTSLMapFunction;

int RTSLApp__init(RTSLApp *_app);
int RTSLApp__run(RTSLApp *_app);
int RTSLApp__close(RTSLApp *_app);

int RTSLAppFunction__encrypt(void *_app);
int RTSLAppFunction__decrypt(void *_app);
int RTSLAppFunction__help(void *_app);

#endif