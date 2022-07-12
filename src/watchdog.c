#include <watchdog.h>
#include <warehouse.h>
#include <thread_sleep.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>

#define N_ROUNDS 10
#define WATCHDOG_SLEEP 2000 / N_ROUNDS

void* watchdog(void* arg) {
    Warehouse* w = *(Warehouse**)arg;
    int reader_tmp;
    int analyser_tmp;
    int printer_tmp;
    int logger_tmp;
    size_t reader_cnt = 0;
    size_t analyser_cnt = 0;
    size_t printer_cnt = 0;
    size_t logger_cnt = 0;

    sleep(1);

    warehouse_thread_put_to_logger(w, "[WATCHDOG] Entering main loop", info);

    while(!warehouse_watchdog_is_done() && reader_cnt < N_ROUNDS && analyser_cnt < N_ROUNDS && printer_cnt < N_ROUNDS && logger_cnt < N_ROUNDS) {
        thread_sleep_millis(WATCHDOG_SLEEP);

        reader_tmp = warehouse_watchdog_check_reader(w);
        if(reader_tmp != -1) {
            reader_cnt = (reader_cnt + 1) * (size_t)reader_tmp;
        }

        analyser_tmp = warehouse_watchdog_check_analyser(w);
        if(analyser_tmp != -1) {
            analyser_cnt = (analyser_cnt + 1) * (size_t)analyser_tmp;
        }

        printer_tmp = warehouse_watchdog_check_printer(w);
        if(printer_tmp != -1) {
            printer_cnt = (printer_cnt + 1) * (size_t)printer_tmp;
        }

        logger_tmp = warehouse_watchdog_check_logger(w);
        if(logger_tmp != -1) {
            logger_cnt = (logger_cnt + 1) * (size_t)logger_tmp;
        }
    }

    if(reader_cnt >= N_ROUNDS || analyser_cnt >= N_ROUNDS || printer_cnt >= N_ROUNDS || logger_cnt >= N_ROUNDS) {
        warehouse_thread_put_to_logger(w, "[WATCHDOG] Exited after worker thread has not responed in 2 seconds", exit_info);
    } else {
        warehouse_thread_put_to_logger(w, "[WATCHDOG] Exited main loop", exit_info);
    }

    printf("Terminating...\n");

    watchdog_terminate_threads();

    return NULL;
} 
