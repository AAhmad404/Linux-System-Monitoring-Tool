# System Monitoring Tool

System Monitoring Tool to report different metrics of a Linux system.

## Metadata

Author: Aqeeb Ahmad

## Features

- Memory utilization chart

- CPU utilization chart

- CPU cores visual with total cores and max clock information

- Custom time delay/sample settings

- Concurrent data collection

- Signal interruption

## Usage Guide

1. Compile the program

```bash
make
```

2. Run the program

```bash
./myMonitoringTool [samples [tdelay]] [--memory] [--cpu] [--cores] [--samples=N] [--tdelay=T]
```

- N must be more than 0
- T must be more than 1000 and less than 1000000

If no arguments are provided, the program will run with the following by default.

```bash
./myMonitoringTool 20 500000 --memory --cpu --cores
```

**Example 1:** Show all components (memory/cpu usage, core info) for 50 samples at a delay of 1 second.

```bash
./myMonitoringTool --memory --cpu --cores --tdelay=1000000 --samples=50
```

**Example 2:** Only show cpu related information (usage, core info)

```bash
./myMonitoringTool --cpu --cores
```

**Example 3:** Only show core information

```bash
./myMonitoringTool --cores
```

Also note that for this case, if the user decided to provide sample and tdelay arguments, they would be ignored as the core diagram is static.

**Interupt the Program**

```bash
Ctrl + C
```

The program will ask for confirmation. If y is entered the program will terminate, otherwise the program continue where it left off.

**Note**

If you face any issues see [Issues](#issues-you-may-face)

### Argument Guide

- --memory: Display the memory usage chart.

- --cpu: Display the CPU usage chart.

- --cores: Use this to display the core diagram, which also displays the number of cores along with the max frequency of the processor.

- --samples=N: Specify the number of samples taken. In other words, the number of times the CPU and memory charts will be updated before the program terminates.

- N must be more than 0.

- 20 samples are taken by default.

- Ignored if the user chooses to just display the cores with --cores.

- --tdelay=T: The time delay (in microseconds) between each CPU and memory chart update.

- Must be more than 1000, but note that even at lower delay values, the chart may not be rendered correctly.

- 500000 microseconds is the default.

- Ignored if the user chooses to just display the cores with --cores.

## Issues you may face

The chart is not rendering correctly; please try the following:

- Increase the size of the terminal.

- Reduce the number of samples you want to see so that the chart fits within the terminal.

- Increase the tdelay.    

## Disclaimers

**Usage**

- The user should provide the program with enough terminal space, height, and width based on the number of samples and components they want to see. Not doing so may result in the charts rendering incorrectly in the terminal.

**CPU**

- CPU utilization is calculated using active CPU time / total CPU time over the time interval specified by the user (tdelay).

- Active CPU time is the total CPU time minus idle CPU time.

- Idle CPU is defined as (4) on [manual](https://man7.org/linux/man-pages/man5/proc_stat.5.html).

- I have assumed CPU0 runs at the cpus highest max frequency.

- Memory usage is calculated as total memory - free memory.

