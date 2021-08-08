#pragma once

#include <time.h>

#include "defines.h"

typedef struct stopwatch {
    clock_t start;
    clock_t end;
    clock_t elapsed;
    bool running;
} stopwatch;

void watch_start(stopwatch* watch);
void watch_stop(stopwatch* watch);
clock_t watch_elapsed(stopwatch* watch);
void watch_reset(stopwatch* watch);