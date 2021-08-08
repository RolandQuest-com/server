#include "stopwatch.h"



void watch_start(stopwatch* watch){
    if(!watch->running){
        watch->start = clock();
        watch->running = true;
    }
}

void watch_stop(stopwatch* watch){
    if(watch->running){
        watch->end = clock();
        watch->elapsed += watch->end - watch->start;
        watch->running = false;
    }
}

clock_t watch_elapsed(stopwatch* watch){
    clock_t ret = watch->elapsed;
    if(watch->running){
        ret += clock() - watch->start;
    }
    return ret;
}

void watch_reset(stopwatch* watch){
    watch->end = 0;
    watch->elapsed = 0;
    watch->running = false;
    watch->start = 0;
}