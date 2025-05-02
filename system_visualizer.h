#ifndef SYSTEM_VISUALIZER
#define SYSTEM_VISUALIZER

#include <stdlib.h>
#include <stdio.h>

#define TOTAL_CPU_TIME_INDEX 0
#define IDLE_CPU_TIME_INDEX 1

#define FIRST_CHART_ORIGIN_ROW 14
#define CHART_ORIGIN_COL 10
#define SIZE_OF_CPU_Y_AXIS 10
#define SIZE_OF_MEMORY_Y_AXIS 12

#define MAX_CPU_USAGE 100

#define GIB_TO_BYTES (1024 * 1024 * 1024)
#define SEC_TO_MS_FACTOR 1000000

// Argument array values
#define MEMORY_ARG 0
#define CPU_ARG 1
#define CORES_ARG 2
#define SAMPLES_ARG 3
#define TDELAY_ARG 4

// The space each component takes on the console
#define COMPONENT_LINE_DIFF 15

#define GIB_TO_BYTES (1024 * 1024 * 1024)

///_|> descry: Helper function to find get cores per row by minimizing the
///height and
// width difference of the core diagram, making the diagram as square as
// possible.
///_|> num: The number of cpu cores.
///_|> returning: The number of cores that should be displayed per row.
int get_cores_per_row(int num);

///_|> descry: Updates the memory usage chart by adding a data point.
///_|> sample: Current sample number. Used to determine data point column.
///_|> chart_position_row: y-axis location of the chart (origin)
///_|> chart_position_col: x-axis location of the chart (origin)
///_|> total_ram: total ram the in system (bytes)
///_|> free_ram: free ram in the system (bytes)
///_|> returning: Nothingn is returned.
void update_memory_chart(int sample, int chart_position_row,
						 int chart_position_col, unsigned long total_ram,
						 unsigned long free_ram);

///_|> descry: Updates the cpu usage chart by adding a data point.
///_|> sample: Current sample number. Used to determine data point column.
///_|> chart_position_row: y-axis location of the chart (origin)
///_|> chart_position_col: x-axis location of the chart (origin)
///_|> prev_cpu_time_data: the previous cpu usage data with the total
/// and idle time. This data is used to calculate the new interval.
///_|> curr_cpu_time_data: The current cpu usage data with the total
/// and idle time.
///_|> returning: Nothing is returned.
void update_cpu_chart(int sample, int chart_position_row, int chart_position_col, unsigned long* prev_cpu_time_data, unsigned long* curr_cpu_time_data);

///_|> descry: Updates the cpu and memory usage charts based on arguments
/// 'arguments'
///_|> sample: Current sample number. Used to determine data point column.
///_|> total_ram: total ram the in system (bytes)
///_|> free_ram: free ram in the system (bytes)
///_|> prev_cpu_time_data: the previous cpu usage data with the total
/// and idle time. This data is used to calculate the new interval.
///_|> curr_cpu_time_data: The current cpu usage data with the total
/// and idle time.
///_|> returning: Nothing is retruned.
void update_charts(int* arguments, int sample, unsigned long total_ram, 
	unsigned long free_ram, unsigned long* prev_cpu_time_data,
	unsigned long* curr_cpu_time_data);

///_|> descry: Displays the core diagram along with the number of cores and
/// max frequency.
///_|> cores: The number of cores to be displayed.
///_|> max_clock: The max clock of the system.
///_|> returning: Nothing is returned.
void display_cores(int cores, double max_clock);

///_|> descry: Displays the initial memory usage chart with x-axis size
/// based on the number of samples.
///_|> samples: The number of samples that the chart should be able to
/// handle.
///_|> total_ram: The total ram of the system which is used to display the
/// max ram on the y-axis of the chart.
///_|> arg: The argument to be parsed.
///_|> returning: Nothing is returned.
void display_base_memory_chart(int samples, unsigned long total_ram);

///_|> descry: Displays the base cpu usage chart with x-axis size based
/// on the number of samples.
///_|> samples: The number of samples that the chart should be able to
/// handle.
///_|> returning: Nothing is returned.
void display_base_cpu_chart(int samples);

///_|> descry: Displays the initial cpu usage chart, memory usage chart, num
/// of samples and time delay based on the given arguments.
///_|> arguments: An array that track the arguments passed into the
/// program by the user.
///_|> returning: Nothing is returned.
void display_base_charts(int* arguments, unsigned long total_ram);

#endif
