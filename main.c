#define _DEFAULT_SOURCE

// Libraries
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "sender.h"
#include "system_visualizer.h"

// Constants
#define MEMORY_FLAG "--memory"
#define CPU_FLAG "--cpu"
#define CORES_FLAG "--cores"
#define SAMPLE_FLAG "--samples="
#define TDELAY_FLAG "--tdelay="

#define MEMORY_FLAG_LEN 8
#define CPU_FLAG_LEN 5
#define CORES_FLAG_LEN 7
#define SAMPLE_FLAG_LEN 10
#define TDELAY_FLAG_LEN 9

// Argument array values
#define MEMORY_ARG 0
#define CPU_ARG 1
#define CORES_ARG 2
#define SAMPLES_ARG 3
#define TDELAY_ARG 4

// Pipes
#define READ 0
#define WRITE 1

// Pipe numbers
#define ONE 0
#define TWO 1
#define THREE 2
#define FOUR 3

#define PROC_COUNT 4

// Default values
#define DEFAULT_SAMPLES 20
#define DEFAULT_TDELAY 500000

#define NONE -1

// Function Prototypes

///_|> descry: SIGINT signal handler for the child processes.
///_|> returning: Nothing is returned.
void run_sig_checks(int* proc_pids);

///_|> descry: Create child processes for the program.
///_|> fd: The pipe the new process will use to communicate to the main
/// process.
///_|> returning: The pid of the new process.
int create_process(int* fd);

///_|> descry: Create all processes that will be needed by the program to
/// collect data
/// based on arguments 'arguments'.
///_|> proc_pids: pids of all the processes created by the main process.
///_|> fd: The pipe that will be used to communicate data between the parent
/// and child processes.
///_|> arguments: An array of argument values which have been modified
/// based on the arguments provided by the user.
///_|> returning: Nothing is returned.
void create_all_processes(int parent_pid, int proc_pids[PROC_COUNT],
						  int fd[PROC_COUNT][2], int* arguments);

///_|> descry: Display the data collected using the pipes.
///_|> proc_pids: pids of all the processes created by the main process.
///_|> fd: The pipes with data to display
///_|> arguments: An array of argument values which have been modified
/// based on the arguments provided by the user.
///_|> returning: Nothing is returned.
void display_data(int proc_pids[PROC_COUNT], int fd[PROC_COUNT][2],
				  int* arguments);

///_|> descry: Collect neccessary data for the main process
///_|> proc_pids: pids of all the processes created by the main process.
///_|> fd: The pipe the new process will use to communicate to the main
/// process.
///_|> arguments: An array of argument values which have been modified
/// based on the arguments provided by the user.
///_|> returning: Nothing is returned.
void collect_data(int proc_pids[PROC_COUNT], int fd[PROC_COUNT][2],
				  int* arguments);

///_|> descry: Collect exit codes of all child processes created by the
/// program.
///_|> returning: 0, if all processes exited with a exit code of 0,
/// otherwise 1.
int collect_exit_codes(int* proc_pids);

///_|> descry: Checks to see if the 'arg' communicates that the memory usage
/// chart should be displayed.
///_|> show_memory: true, if the chart is already set to be outputted,
/// otherwise false.
///_|> arg: The argument to be parsed.
///_|> returning: true, if 'arg' communicates that the chart should be
/// outputted, otherwise false.
bool set_memory_usage(bool show_memory_usage, char* arg);

///_|> descry: Checks to see if the 'arg' communicates that the cpu usage
/// chart should be displayed.
///_|> show_cpu_usage: true, if the chart is already set to be outputted,
/// otherwise false.
///_|> arg: The argument to be parsed.
///_|> returning: true, if 'arg' communicates that the table should be
/// outputted, otherwise false.
bool set_cpu_usage(bool show_cpu_usage, char* arg);

///_|> descry: Checks to see if the 'arg' communicates that the cores
/// diagram should be displayed
///_|> show_cores: true, if the cores diagram is already set to be
///displayed,
/// otherwise false.
///_|> arg: The argument to be parsed.
///_|> returning: true, if 'arg' communicates that the diagram should be
/// outputted, otherwise false.
bool set_cores(bool show_cores, char* arg);

///_|> descry: Checks to see if the 'arg' communicates the default sample
/// count should be changed.
///_|> sample_set: true, if the default sample count should be changed.
///_|> arg: The argument to be parsed.
///_|> returning: true, if 'arg' communicates that the default number of
/// samples should be changed, otherwise false.
bool set_samples(bool sample_set, char* arg);

///_|> descry: Checks to see if the 'arg' communicates the default tdelay
/// should be changed.
///_|> sample_set: true, if the default tdelay should be changed.
///_|> arg: The argument to be parsed.
///_|> returning: true, if 'arg' communicates that the default tdelay
/// should be changed, otherwise false.
bool set_tdelay(bool tdelay_set, char* arg);

///_|> descry: Set values to be used while deciding what to display based on
/// the arguments passed when running the executable.
///_|> argc: The number of arguments.
///_|> argv: The arguments passed when running the executable file.
///_|> arguments: An array of argument values which are modified
/// based on the arguments provided by the user in 'argv'.
///_|> returning: Nothing is returned by the function.
void set_arguments(int argc, char** argv, int* arguments);

// Signal handlers
volatile sig_atomic_t sig_atomic = 0;

void handler_ctrl_c_parent() {
	///_|> descry: SIGINT signal handler for the main process.
	///_|> returning: Nothing is returned.

	char selection[2];

	printf("\x1b[%d;%df", 35, 0);
	printf("Would you like to quit the program? (y/n) \n");

	fgets(selection, sizeof(selection), stdin);

	// Hide the question
	printf("\x1b[%d;%df", 35, 0);
	printf(
		"                                           \n                         "
		"                  ");

	if (strcmp("y", selection) == 0) {
		sig_atomic = 1;
		return;
	}

	scanf("%*[^\n]");
	scanf("%*c");
}

void handler_ctrl_c_child() {
	///_|> descry: SIGINT signal handler for the child processes.
	///_|> returning: Nothing is returned.

	kill(getpid(), SIGSTOP);
}

/* Function implementations */

void run_sig_checks(int* proc_pids) {
	if (sig_atomic == 0) {
		if (proc_pids[ONE] != NONE) {
			kill(proc_pids[ONE], SIGCONT);
		}

		if (proc_pids[TWO] != NONE) {
			kill(proc_pids[TWO], SIGCONT);
		}
	}

	if (sig_atomic == 1) {
		if (proc_pids[ONE] != NONE) {
			kill(proc_pids[ONE], SIGKILL);

			proc_pids[ONE] = NONE;
		}

		if (proc_pids[TWO] != NONE) {
			kill(proc_pids[TWO], SIGKILL);

			proc_pids[TWO] = NONE;
		}

		int error = collect_exit_codes(proc_pids);

		// Move cursor the bottom of the screen
		printf("\x1b[%d;%df", 99, 0);

		exit(error);
	}
}

int create_process(int* fd) {
	int pid = fork();

	if (pid == 0) {
		close(fd[READ]);
	} else if (pid > 0) {
		close(fd[WRITE]);
	}

	if (pid == NONE) {
		fprintf(stderr, "Error occured while creating a process.");
		exit(1);
	}

	return pid;
}

void create_all_processes(int parent_pid, int proc_pids[PROC_COUNT],
						  int fd[PROC_COUNT][2], int* arguments) {
	if (arguments[MEMORY_ARG]) {
		proc_pids[ONE] = create_process(fd[ONE]);
	}

	if (arguments[CPU_ARG] && parent_pid == getpid()) {
		proc_pids[TWO] = create_process(fd[TWO]);
	}

	if (arguments[CORES_ARG] && parent_pid == getpid()) {
		proc_pids[THREE] = create_process(fd[THREE]);
	}

	if (arguments[CORES_ARG] && parent_pid == getpid()) {
		proc_pids[FOUR] = create_process(fd[FOUR]);
	}
}

void display_data(int proc_pids[PROC_COUNT], int fd[PROC_COUNT][2],
				  int* arguments) {
	unsigned long prev_cpu_time_data[2];

	// If the neither memory usage and cpu usage is selected ignore sample and
	// tdelay values
	if ((arguments[MEMORY_ARG] || arguments[CPU_ARG])) {
		printf("\x1b[%d;%df", 1, 1);

		if (arguments[CPU_ARG]) {
			read(fd[TWO][READ], prev_cpu_time_data, sizeof(unsigned long) * 2);
		}

		if (arguments[MEMORY_ARG]) {
			unsigned long total_ram;
			read(fd[ONE][READ], &total_ram, sizeof(unsigned long));
			display_base_charts(arguments, total_ram);
		} else {
			display_base_charts(arguments, -1);
		}

		for (int sample = 1; sample <= arguments[SAMPLES_ARG]; sample++) {
			usleep(arguments[TDELAY_ARG]);

			run_sig_checks(proc_pids);

			unsigned long total_ram;
			unsigned long free_ram;

			unsigned long curr_cpu_time_data[2];

			// Read memory usage data
			if (arguments[MEMORY_ARG]) {
				read(fd[ONE][READ], &total_ram, sizeof(unsigned long));
				read(fd[ONE][READ], &free_ram, sizeof(unsigned long));
			}

			// Read cpu usage data.
			if (arguments[CPU_ARG]) {
				read(fd[TWO][READ], curr_cpu_time_data,
					 sizeof(unsigned long) * 2);
			}

			update_charts(arguments, sample, total_ram, free_ram,
						  prev_cpu_time_data, curr_cpu_time_data);

			// Update the previous cpu time to be used in the next interation.
			prev_cpu_time_data[TOTAL_CPU_TIME_INDEX] =
				curr_cpu_time_data[TOTAL_CPU_TIME_INDEX];
			prev_cpu_time_data[IDLE_CPU_TIME_INDEX] =
				curr_cpu_time_data[IDLE_CPU_TIME_INDEX];
		}
	}

	// Display max frequency and the cores diagram.
	if (arguments[CORES_ARG]) {
		int row = 1;

		if (arguments[MEMORY_ARG]) row = row + COMPONENT_LINE_DIFF + 2;
		if (arguments[CPU_ARG]) row = row + COMPONENT_LINE_DIFF;

		// Make the spacing more compact
		if (arguments[CPU_ARG] && arguments[MEMORY_ARG]) row = row - 2;

		printf("\x1b[%d;%df", row, 1);

		double max_clock;
		int cores;

		read(fd[THREE][READ], &max_clock, sizeof(double));
		read(fd[FOUR][READ], &cores, sizeof(int));

		display_cores(cores, max_clock);
	}
}

void collect_data(int proc_pids[PROC_COUNT], int fd[PROC_COUNT][2],
				  int* arguments) {
	if (proc_pids[ONE] == 0) {
		send_memory_data_to_main(fd[ONE], arguments[SAMPLES_ARG],
								 arguments[TDELAY_ARG]);
		exit(0);
	}

	if (proc_pids[TWO] == 0) {
		send_cpu_data_to_main(fd[TWO], arguments[SAMPLES_ARG],
							  arguments[TDELAY_ARG]);
		exit(0);
	}

	if (proc_pids[THREE] == 0) {
		send_max_clock_to_main(fd[THREE]);
		exit(0);
	}

	if (proc_pids[FOUR] == 0) {
		send_num_of_cores_to_main(fd[FOUR]);
		exit(0);
	}
}

int collect_exit_codes(int* proc_pids) {
	int status;
	int error = 0;

	for (int i = 0; i < 4; i++) {
		if (proc_pids[i] == NONE) {
			continue;
		}

		waitpid(proc_pids[i], &status, 0);

		// Check is a child process exited without an error.
		if (status != 0) {
			fprintf(stderr, "Child %d exited abnormally.\n", proc_pids[i]);

			// Error found
			error = 1;
		}
	}

	return error;
}

bool set_memory_usage(bool show_memory_usage, char* arg) {
	return !show_memory_usage && strcmp(arg, MEMORY_FLAG) == 0;
}

bool set_cpu_usage(bool show_cpu_usage, char* arg) {
	return !show_cpu_usage && strcmp(arg, CPU_FLAG) == 0;
}

bool set_cores(bool show_cores, char* arg) {
	return !show_cores && strcmp(arg, CORES_FLAG) == 0;
}

bool set_samples(bool sample_set, char* arg) {
	return !sample_set && strncmp(arg, SAMPLE_FLAG, SAMPLE_FLAG_LEN) == 0;
}

bool set_tdelay(bool tdelay_set, char* arg) {
	return !tdelay_set && strncmp(arg, TDELAY_FLAG, TDELAY_FLAG_LEN) == 0;
}

void set_arguments(int argc, char** argv, int* arguments) {
	bool sample_set = false;
	bool tdelay_set = false;

	for (int i = 1; i < argc; i++) {
		if (set_memory_usage(arguments[MEMORY_ARG], argv[i])) {
			arguments[MEMORY_ARG] = true;
		} else if (set_cpu_usage(arguments[CPU_ARG], argv[i])) {
			arguments[CPU_ARG] = true;
		} else if (set_cores(arguments[CORES_ARG], argv[i])) {
			arguments[CORES_ARG] = true;
		} else if (set_samples(sample_set, argv[i])) {
			char val[20];
			strncpy(val, argv[i] + SAMPLE_FLAG_LEN, 20);
			arguments[SAMPLES_ARG] = atoi(val);
			sample_set = true;

			if (arguments[SAMPLES_ARG] <= 0) {
				fflush(stdout);
				fprintf(stderr, "Invalid samples value: %s\n", val);
				exit(1);
			}
		} else if (set_tdelay(tdelay_set, argv[i])) {
			char val[20];
			strncpy(val, argv[i] + TDELAY_FLAG_LEN, 20);
			arguments[TDELAY_ARG] = atoi(val);
			tdelay_set = true;

			// Make sure thre tdelay value is more than zero
			if (arguments[TDELAY_ARG] <= 1000) {
				fflush(stdout);
				fprintf(
					stderr,
					"Invalid tdelay value: %s. Must be between 1000-1000000.\n",
					val);
				exit(1);
			}

		} else if (atoi(argv[i]) != 0) {  // Check for non-flagged arguments
			if (!sample_set) {
				arguments[SAMPLES_ARG] = atoi(argv[i]);
				sample_set = true;
			} else if (!tdelay_set) {
				arguments[TDELAY_ARG] = atoi(argv[i]);
				tdelay_set = true;
			} else {
				fflush(stdout);
				fprintf(stderr, "Repeated argument: %s\n", argv[i]);
				exit(1);
			}
		} else {
			fflush(stdout);
			fprintf(stderr, "Invalid or repeated argument: %s\n", argv[i]);
			exit(1);
		}
	}

	// Set default arguments
	if (!arguments[MEMORY_ARG] && !arguments[CPU_ARG] &&
		!arguments[CORES_ARG]) {
		arguments[MEMORY_ARG] = true;
		arguments[CPU_ARG] = true;
		arguments[CORES_ARG] = true;
	}
}

int main(int argc, char** argv) {
	///_|> argc: The number of arguments
	///_|> argv: The arguments passed when running the executable file.

	int parent_pid = getpid();

	bool show_memory_usage = false;
	bool show_cores = false;
	bool show_cpu_usage = false;

	int samples = DEFAULT_SAMPLES;
	int tdelay = DEFAULT_TDELAY;

	int arguments[5] = {show_memory_usage, show_cpu_usage, show_cores, samples,
						tdelay};
	set_arguments(argc, argv, arguments);

	// Array to store all the communication pipes
	int fd[PROC_COUNT][2];

	if (pipe(fd[ONE]) == NONE || pipe(fd[TWO]) == NONE ||
		pipe(fd[THREE]) == NONE || pipe(fd[FOUR]) == NONE) {
		fprintf(stderr, "Error occured while creating a pipe.");
		exit(1);
	}

	// Store the pids of the child processes
	int proc_pids[PROC_COUNT] = {NONE, NONE, NONE, NONE};

	// Ignore the stop signal
	signal(SIGTSTP, SIG_IGN);

	// Create all processes
	create_all_processes(parent_pid, proc_pids, fd, arguments);

	// Set seperate SIGINT handlers for the parent and child processes
	if (parent_pid == getpid()) {
		struct sigaction newact2;
		newact2.sa_handler = handler_ctrl_c_parent;
		newact2.sa_flags = 0;
		sigemptyset(&newact2.sa_mask);

		if (sigaction(SIGINT, &newact2, NULL) == -1) {
			fprintf(stderr, "Error: sigaction");
			return 1;
		}
	} else {
		struct sigaction newact2;
		newact2.sa_handler = handler_ctrl_c_child;
		newact2.sa_flags = 0;
		sigemptyset(&newact2.sa_mask);

		if (sigaction(SIGINT, &newact2, NULL) == -1) {
			fprintf(stderr, "Error: sigaction");
			return 1;
		}
	}

	// Collect data
	collect_data(proc_pids, fd, arguments);

	// Clear the screen
	printf("\033[2J");

	// Display the data being collected.
	display_data(proc_pids, fd, arguments);

	// Close read pipes
	close(fd[ONE][READ]);
	close(fd[TWO][READ]);
	close(fd[THREE][READ]);
	close(fd[FOUR][READ]);

	// Move cursor the bottom of the screen
	printf("\x1b[%d;%df", 99, 0);

	int error = collect_exit_codes(proc_pids);

	return error;
}
