#include "sender.h"

void send_memory_data_to_main(int* fd, int num_of_samples, int tdelay) {
	struct sysinfo info;
	sysinfo(&info);

	unsigned long total_ram;
	unsigned long free_ram;

	total_ram = info.totalram;
	write(fd[WRITE], &total_ram, sizeof(unsigned long));

	for (int sample = 1; sample <= num_of_samples; sample++) {
		usleep(tdelay);

		struct sysinfo info;
		sysinfo(&info);

		total_ram = info.totalram;
		free_ram = info.freeram;

		// Send data to the main process
		write(fd[WRITE], &total_ram, sizeof(unsigned long));
		write(fd[WRITE], &free_ram, sizeof(unsigned long));
	}

	close(fd[WRITE]);
}

void send_cpu_data_to_main(int* fd, int num_of_samples, int tdelay) {
	unsigned long cpu_time_data[2];

	set_cpu_time_data(cpu_time_data);

	// Send data to the main process
	write(fd[WRITE], cpu_time_data, sizeof(unsigned long) * 2);

	for (int sample = 1; sample <= num_of_samples; sample++) {
		usleep(tdelay);
		unsigned long cpu_time_data[2];

		set_cpu_time_data(cpu_time_data);
		write(fd[WRITE], cpu_time_data, sizeof(unsigned long) * 2);
	}

	close(fd[WRITE]);
}

void send_max_clock_to_main(int* fd) {
	double max_clock = retrieve_max_clock();
	write(fd[WRITE], &max_clock, sizeof(double));
	close(fd[WRITE]);
}

void send_num_of_cores_to_main(int* fd) {
	int cores = get_nprocs_conf();
	write(fd[WRITE], &cores, sizeof(int));
	close(fd[WRITE]);
}
