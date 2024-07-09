#ifndef RATSEL_H
#define RATSEL_H

#define RATSEL_MAJOR 1
#define RATSEL_MINOR 0
#define RATSEL_PATCH 1

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <malloc.h>

#include "hashmap/hashmap.h"
#include "neda.h"

#define RTSL_UNUSED(_v) (void)(_v);
#define RTSL_SUCCESS 0
#define RTSL_FAILURE 1
#define RTSL_COMMAND_LINE_MAX 512
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

typedef char * RTSLString;

typedef struct RTSLBuffer
{
    unsigned char *data;
    size_t size;
} RTSLBuffer;

NEDA_HEADER(RTSLString);

typedef struct
{
    struct hashmap *functions;
    neda_RTSLString *args;
} RTSLApp;

typedef struct
{
    char *name;
    RTSLFunction function;
} RTSLMapFunction;

int RTSLApp__init(RTSLApp *_app);
int RTSLApp__run(RTSLApp *_app);
void RTSLApp__close(RTSLApp *_app);

int RTSLAppFunction__encrypt(void *_app);
int RTSLAppFunction__decrypt(void *_app);
int RTSLAppFunction__help(void *_app);

#endif