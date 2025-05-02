#include "system_data.h"

double retrieve_max_clock() {
	FILE* freq_file = NULL;
	char line[LINE_LENGTH + 1];
	int cpu_freq = 0;

	// Read file
	freq_file = fopen(CPUINFO_MAX_FREQ, "r");
	if (freq_file == NULL) {
		fflush(stdout);
		fprintf(stderr, "Error Opening file: %s\n", CPUINFO_MAX_FREQ);
		exit(1);
	}

	// Retrive max frequency
	while (fgets(line, LINE_LENGTH + 1, freq_file) != NULL) {
		cpu_freq = atoi(line);

		if (cpu_freq != 0) {
			break;
		}
	}

	if (fclose(freq_file) != 0) {
		fflush(stdout);
		fprintf(stderr, "fclose failed\n");
		exit(1);
	}

	if (cpu_freq == 0) {
		fflush(stdout);
		fprintf(
			stderr,
			"An error occured while retrieving cpu frequency information.\n");
		exit(1);
	}
	return (double)cpu_freq / MHZ_TO_HZ_FACTOR;
}

void set_cpu_time_data(unsigned long* cpu_time_data) {
	FILE* cpu_stats_file = NULL;

	// Array to store the different time types in the proc_stat file.
	unsigned long time[10];

	// Temporary array to store the name of the of cpu.
	char cpu_name[10];

	// Read file
	cpu_stats_file = fopen(PROC_STAT, "r");
	if (cpu_stats_file == NULL) {
		fflush(stdout);
		fprintf(stderr, "Error Opening file: %s\n", PROC_STAT);
		exit(1);
	}

	// Read the first line of the file
	int status =
		fscanf(cpu_stats_file, "%10s %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu",
			   cpu_name, time + USER_TIME_INDEX, time + NICE_TIME_INDEX,
			   time + SYSTEM_TIME_INDEX, time + IDLE_TIME_INDEX,
			   time + IOWAIT_TIME_INDEX, time + IRQ_TIME_INDEX,
			   time + SOFTIRQ_TIME_INDEX, time + STEAL_TIME_INDEX,
			   time + GUEST_TIME_INDEX, time + GUEST_NICE_TIME_INDEX);

	// Check if all eleven (10 time) values in the line were read.
	// @see https://man7.org/linux/man-pages/man5/proc_stat.5.html
	if (status != 11) {
		fflush(stdout);
		fprintf(stderr, "Error occured while reading filed: %d instead of 7\n",
				status);
		exit(1);
	}

	int error = fclose(cpu_stats_file);

	if (error != 0) {
		fflush(stdout);
		fprintf(stderr, "fclose failed on input file\n");
		exit(1);
	}

	// Set the cpu_time_data array with the total and idle cpu times.
	cpu_time_data[TOTAL_CPU_TIME_INDEX] =
		time[USER_TIME_INDEX] + time[NICE_TIME_INDEX] +
		time[SYSTEM_TIME_INDEX] + time[IDLE_TIME_INDEX] +
		time[IOWAIT_TIME_INDEX] + time[IRQ_TIME_INDEX] +
		time[SOFTIRQ_TIME_INDEX] + time[STEAL_TIME_INDEX] +
		time[GUEST_TIME_INDEX] + time[GUEST_NICE_TIME_INDEX];
	cpu_time_data[IDLE_CPU_TIME_INDEX] = time[IDLE_TIME_INDEX];
}
