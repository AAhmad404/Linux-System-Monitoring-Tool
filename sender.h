#ifndef SENDER
#define SENDER

#include <unistd.h>
#include "system_data.h"

#define READ 0
#define WRITE 1

///_|> descry: Sends memory usage data the main processes using the pipe
///'fd'.
///_|> fd: Pipe to communicate to the main process.
///_|> num_of_samples: The number of samples that need to be taken.
///_|> tdelay: The delay between each sample.
///_|> returning: Nothing is returned.
void send_memory_data_to_main(int* fd, int num_of_samples, int tdelay);

///_|> descry: Sends the cpu usage data the main processes using the pipe
///'fd'
///_|> fd: Pipe to communicate to the main process.
///_|> num_of_samples: The number of samples that need to be taken.
///_|> tdelay: The delay between each sample.
///_|> returning: Nothing is returned.
void send_cpu_data_to_main(int* fd, int num_of_samples, int tdelay);

///_|> descry: Sends the max clock of the cpu to the main process.
///_|> fd: Pipe to communicate to the main process.
///_|> returning: Nothing is returned.
void send_max_clock_to_main(int* fd);

///_|> descry: Sends the number cores to the main process.
///_|> fd: Pipe to communicate to the main process.
///_|> returning: Nothing is returned.
void send_num_of_cores_to_main(int* fd);

#endif
