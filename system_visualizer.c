#include "system_visualizer.h"

int get_cores_per_row(int num) {
	int mid = num / 2;
	int cores_per_row = 1;

	int smallest_diff = num;
	int diff;

	for (int i = 2; i <= mid; i++) {
		if (num % i == 0) {
			// Calcualte the difference between the height and width
			diff = abs(i - (num / i));

			if (diff <= smallest_diff) {
				cores_per_row = i;
				smallest_diff = diff;
			}
		}
	}

	return cores_per_row;
}

void update_memory_chart(int sample, int chart_position_row,
						 int chart_position_col, unsigned long total_ram,
						 unsigned long free_ram) {
	float total_ram_gib = (float)total_ram / (GIB_TO_BYTES);
	float free_ram_gib = (float)free_ram / (GIB_TO_BYTES);
	float ram_usage_gib = total_ram_gib - free_ram_gib;

	// Calculate the chart row to edit and round the nearest row
	int target_row =
		(int)(((SIZE_OF_MEMORY_Y_AXIS / total_ram_gib) * ram_usage_gib) + 0.5);

	// Move to the top of the chart and edit the cpu usage title value
	printf("\x1b[%d;%df", chart_position_row - 11, 1);
	printf("v Memory %4.2f GB           \n", ram_usage_gib);

	// Display the data point symbol at row 1 if the value rounds to 0.
	if (target_row == 0) {
		target_row = 1;
	}

	// Add data points to the chart
	printf("\x1b[%d;%df", chart_position_row - target_row + 2,
		   chart_position_col + sample);
	printf("#");

	// Empty buffer
	fflush(stdout);
	printf("\033[H");
}

void update_cpu_chart(int sample, int chart_position_row,
					  int chart_position_col, unsigned long* prev_cpu_time_data,
					  unsigned long* curr_cpu_time_data) {
	unsigned long new_total_cpu_time = curr_cpu_time_data[TOTAL_CPU_TIME_INDEX];
	unsigned long new_idle_cpu_time = curr_cpu_time_data[IDLE_CPU_TIME_INDEX];

	// Calculate the time for the current interval
	unsigned long total_interval_time =
		new_total_cpu_time - prev_cpu_time_data[TOTAL_CPU_TIME_INDEX];
	unsigned long idle_interval_time =
		new_idle_cpu_time - prev_cpu_time_data[IDLE_CPU_TIME_INDEX];

	double cpu_usage = (double)MAX_CPU_USAGE / total_interval_time *
					   (total_interval_time - idle_interval_time);

	// Calculate the chart row to edit and round to the nearest row
	int target_row =
		(int)(((float)SIZE_OF_CPU_Y_AXIS / MAX_CPU_USAGE * cpu_usage) + 0.5);

	// Move to the top of the chart and edit the cpu usage title value
	printf("\x1b[%d;%df", chart_position_row - SIZE_OF_CPU_Y_AXIS - 1, 1);
	printf("v CPU %4.2f %%  ", cpu_usage);

	// Display the data point symbol at row 1 if the value rounds to 0.
	if (target_row == 0) {
		target_row = 1;
	}

	// Add data points to the chart
	printf("\x1b[%d;%df", chart_position_row - target_row,
		   chart_position_col + sample);
	printf(":");

	// Empty buffer
	fflush(stdout);
	printf("\033[H");

	// Update cpu data
	prev_cpu_time_data[TOTAL_CPU_TIME_INDEX] = new_total_cpu_time;
	prev_cpu_time_data[IDLE_CPU_TIME_INDEX] = new_idle_cpu_time;
}

void update_charts(int* arguments, int sample, unsigned long total_ram,
				   unsigned long free_ram, unsigned long* prev_cpu_time_data,
				   unsigned long* curr_cpu_time_data) {
	int chart_position_row = FIRST_CHART_ORIGIN_ROW;
	int chart_position_col = CHART_ORIGIN_COL;

	if (arguments[MEMORY_ARG]) {
		update_memory_chart(sample, chart_position_row, chart_position_col,
							total_ram, free_ram);
		chart_position_row = chart_position_row + COMPONENT_LINE_DIFF;
	}

	if (arguments[CPU_ARG]) {
		update_cpu_chart(sample, chart_position_row, chart_position_col,
						 prev_cpu_time_data, curr_cpu_time_data);
	}
}

void display_cores(int cores, double max_clock) {
	int cores_per_row = get_cores_per_row(cores);

	printf("\nNumber of Cores: %d @ %.2f GHz", cores, max_clock);

	int i = 0;
	while (i < cores) {
		printf("\n");
		int j;
		for (j = 0; j < cores_per_row && i < cores; i++, j++) {
			printf("+──+ ");
		}
		printf("\n");
		for (int k = 0; k < j; k++) {
			printf("|  | ");
		}
		printf("\n");
		for (int k = 0; k < j; k++) {
			printf("+──+ ");
		}
		printf("\n");
	}
	printf("\n");
}

void display_base_memory_chart(int samples, unsigned long total_ram) {
	float totalram = (float)total_ram / (GIB_TO_BYTES);

	printf("\nv Memory - GB\n");

	// Display rounded max memory
	printf(" %4d GB |\n", (int)(totalram + 0.5));

	// Create y-axis
	for (int i = 1; i < SIZE_OF_MEMORY_Y_AXIS; i++) {
		printf("         |\n");
	}

	printf("   0 GB  ─");

	// Create x-axis
	for (int i = 0; i < samples; i++) {
		printf("─");
	}

	// Empty Buffer
	printf("\n");
}

void display_base_cpu_chart(int samples) {
	printf("\nv CPU - %%\n");
	printf("    %3d%% |\n", MAX_CPU_USAGE);

	// Create y-axis
	for (int i = 1; i < SIZE_OF_CPU_Y_AXIS; i++) {
		printf("         |\n");
	}

	printf("      0%% ─");

	// Create x-axis
	for (int i = 0; i < samples; i++) {
		printf("─");
	}

	// Empty Buffer
	printf("\n");
}

void display_base_charts(int* arguments, unsigned long total_ram) {
	printf("Number of samples: %d -- every %d microSecs (%f secs)\n",
		   arguments[SAMPLES_ARG], arguments[TDELAY_ARG],
		   (double)arguments[TDELAY_ARG] / SEC_TO_MS_FACTOR);

	if (arguments[MEMORY_ARG]) {
		display_base_memory_chart(arguments[SAMPLES_ARG], total_ram);
	}

	if (arguments[CPU_ARG]) {
		display_base_cpu_chart(arguments[SAMPLES_ARG]);
	}
}
