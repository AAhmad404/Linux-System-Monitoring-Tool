#ifndef SYSTEM_DATA
#define SYSTEM_DATA

#include <stdlib.h>
#include <stdio.h>
#include <sys/sysinfo.h>

#define LINE_LENGTH 80

// /proc/stat time value constants
// @see https://man7.org/linux/man-pages/man5/proc_stat.5.html
#define USER_TIME_INDEX 0
#define NICE_TIME_INDEX 1
#define SYSTEM_TIME_INDEX 2
#define IDLE_TIME_INDEX 3
#define IOWAIT_TIME_INDEX 4
#define IRQ_TIME_INDEX 5
#define SOFTIRQ_TIME_INDEX 6
#define STEAL_TIME_INDEX 7
#define GUEST_TIME_INDEX 8
#define GUEST_NICE_TIME_INDEX 9

#define CPUINFO_MAX_FREQ "/sys/devices/system/cpu/cpu0/cpufreq/cpuinfo_max_freq"
#define PROC_STAT "/proc/stat"

#define MHZ_TO_HZ_FACTOR (1000000)

#define TOTAL_CPU_TIME_INDEX 0
#define IDLE_CPU_TIME_INDEX 1

///_|> descry: Retrieves the max freqency of the cpu
///_|> pre-condition: Assumes cpu0 has the highest max clock speed.
///_|> returning: The max clock of the system.
double retrieve_max_clock();

///_|> descry: Retrieves and calculated the total and idle cpus time using
///proc/stat and
/// updates 'cpu_time_data' with that data.
///_|> cpu_time_data: A two index array to store the total and idle time
/// of the cpu.
///_|> returning: Nothing is returned.
void set_cpu_time_data();

#endif
