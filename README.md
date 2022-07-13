# CPU Tracker - C-99 program for analysing CPU usage on Linux systems
Tietoevry recruitment task

## Description
Multi-threaded C program which reads contents of /proc/stat, analyses gathered data and calculates CPU usage.
Featured task-specific threads include:
* Reader - reads contents of /proc/stat file and passes them as custom Message object to Analyser thread.
* Analyser - takes Message passed from Reader and analyses it, calculating usage of each CPU for which there is data available in /proc/stat. Analysed data is passed to Printer thread.
* Printer - takes care of presenting analysed data by printing it in console.
* Watchdog - keeps track of each thread's activity, shuts down the program in case where either of the worker threads has not been active in over 2 seconds.
* Logger - takes informations (for debug purposses etc.) from each task and stores them in file.

Other features include:
* catching SIGTERM to exit safely.

## Compilation and running
This project was built using Makefile. In order to compile with default settings (default compiler is gcc) simply use _make_. To clean project directories from files generated during compilation, use _make clean_. Final compilation output is stored in `main.out` file. To run compiled program, use _./main.out_.

Several variables are supported by Makefile in this project:
* CC - to use a different compiler, run _make CC=compiler_
* V - setting value of variable V to any nonempty string enables verbose mode, e.g. _make V=y_
* O - setting value of variable enables compiler optimisation, e.g. _make O=3_ will run compiler with flag -O3
* G - setting value of variable G allows the use of gdb debugger with the compiled program, e.g. _make G=3_ will run compiler with flag -ggdb3