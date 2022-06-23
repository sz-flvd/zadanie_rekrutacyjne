# CPU Tracker - C-99 program for analysing CPU usage on Linux systems
Tietoevry recruitment task

## Description
Multi-threaded C program which reads contents of /proc/stat, analyses gathered data and calculates CPU usage.
Featured task-specific threads include:
* Reader - reads contents of /proc/stat file and passes them as custom Message object to Analyser thread.
* Analyser - takes Message passed from Reader and analyses it, calculating usage of each CPU for which there is data available in /proc/stat. Analysed data is passed to Printer thread.
* Printer - takes care of presenting analysed data by printing it in console.
* Watchdog - [to be implemented] keeps track of each thread's activity, shuts down the program in case where either of the worker threads has not been active in over 2 seconds.
* Logger - [to be implemented] takes informations (for debug purposses etc.) from each task and stores them in file.

Other features include:
* [to be implemented] catching SIGTERM and SIGINT to exit safely.
.