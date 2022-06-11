#include <stdio.h>
#include <pthread.h>
#include "reader.h"
#include "analyzer.h"
#include "printer.h"
#include "watchdog.h"
#include "logger.h"

int main(int argc, char* argv[]) {
    pthread_t rd, an, pr, wd, lg;

    pthread_create(&rd, NULL, reader, NULL);
    pthread_create(&an, NULL, analyzer, NULL);
    pthread_create(&pr, NULL, printer, NULL);
    pthread_create(&wd, NULL, watchdog, NULL);
    pthread_create(&lg, NULL, logger, NULL);

    pthread_join(rd, NULL);
    pthread_join(an, NULL);
    pthread_join(pr, NULL);
    pthread_join(wd, NULL);
    pthread_join(lg, NULL);

    return 0;
}
