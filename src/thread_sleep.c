#include <thread_sleep.h>
#include <stdlib.h>
#include <time.h>

int thread_sleep_millis(long const millis) {
    struct timespec ts;
    int res;

    if(millis < 0) {
        return -1;
    }

    ts.tv_sec = millis / 1000;
    ts.tv_nsec = (millis % 1000) * 1000000;

    do {
        res = nanosleep(&ts, &ts);
    } while(res != 0);

    return res;
}
