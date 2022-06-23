#include <warehouse.h>
#include <reader.h>
#include <analyser.h>
#include <printer.h>
#include <watchdog.h>
#include <logger.h>
#include <stdio.h>
#include <pthread.h>

int main(int argc, char* argv[]) {
    Warehouse* w = warehouse_create();
    
    pthread_t rd, an, pr, wd, lg;

    pthread_create(&rd, NULL, reader, (void*) &w);
    pthread_create(&an, NULL, analyser, (void*) &w);
    pthread_create(&pr, NULL, printer, (void*) &w);
    // pthread_create(&wd, NULL, watchdog, (void*) &w);
    // pthread_create(&lg, NULL, logger, (void*) &w);

    pthread_join(rd, NULL);
    pthread_join(an, NULL);
    pthread_join(pr, NULL);
    // pthread_join(wd, NULL);
    // pthread_join(lg, NULL);

    warehouse_destroy(w);

    return 0;
}
