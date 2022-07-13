/*  Implementation of main watchdog thread function

    Author: Szymon Przybysz */

#include <watchdog.h>
#include <warehouse.h>
#include <thread_sleep.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>

#define WD_ENTER "[WATCHDOG] Entering main loop"
#define WD_EXIT "[WATCHDOG] Exited main loop"
#define WD_EXIT_TIMEOUT "[WATCHDOG] Exited after worker thread has not responed in 2 seconds"
#define WD_TERM "Terminating...\n"

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

    warehouse_thread_put_to_logger(w, WD_ENTER, info);

    while(!warehouse_watchdog_is_done() && reader_cnt < N_ROUNDS && analyser_cnt < N_ROUNDS && printer_cnt < N_ROUNDS && logger_cnt < N_ROUNDS) {
        thread_sleep_millis(WATCHDOG_SLEEP);

        /*  Counter for each thread is increased by 1 if return value of check function is equal 1
            or reset to 0 if return value is equal 0
            To avoid more if else statements, counter value of each thread is increased by 1 and then
            multiplied by return value of check function of correspondingi thread */
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

    printf(WD_TERM);

    if(reader_cnt >= N_ROUNDS || analyser_cnt >= N_ROUNDS || printer_cnt >= N_ROUNDS || logger_cnt >= N_ROUNDS) {
        warehouse_thread_put_to_logger(w, WD_EXIT_TIMEOUT, exit_info);
        watchdog_terminate_threads();
    } else {
        warehouse_thread_put_to_logger(w, WD_EXIT, exit_info);
    }

    return NULL;
} 
