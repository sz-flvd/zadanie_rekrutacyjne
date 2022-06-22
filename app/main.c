#include <warehouse.h>
#include <reader.h>
#include <analyzer.h>
#include <printer.h>
#include <watchdog.h>
#include <logger.h>
#include <stdio.h>
#include <pthread.h>

int main(int argc, char* argv[]) {
    Warehouse* w = warehouse_create();

    pthread_t rd, an, pr, wd, lg;

    pthread_create(&rd, NULL, reader, w);
    pthread_create(&an, NULL, analyzer, w);
    pthread_create(&pr, NULL, printer, w);
    pthread_create(&wd, NULL, watchdog, w);
    pthread_create(&lg, NULL, logger, w);

    pthread_join(rd, NULL);
    pthread_join(an, NULL);
    pthread_join(pr, NULL);
    pthread_join(wd, NULL);
    pthread_join(lg, NULL);

    return 0;
}
