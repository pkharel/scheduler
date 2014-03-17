#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include "sched.h"
#include <sched.h>
#include <stdlib.h>

/* forward declarations */
void setup_scheduler(char *sched_name);
void setup_options(char *options);
void read_config(const char *config_file);
void run_simulator();
void print_alljobs();

//comparison functions for qsort
int fcfs_cmp(const void *t1,const void *t2);
int nppri_cmp(const void *t1,const void *t2);
int sjf_cmp(const void *t1, const void *t2);
int pid_cmp(const void *t1, const void *t2);

//adds elements arriving at that tick to the queue
void add2queue();
//prints out the final response time and turn around time
//for all jobs
void finalcount();


/* globals */
int tick=0;
task **alljobs;
task *queue[1000];
int q_size=0;
int q_first=0;
int num_jobs;
#define MAX_BUFFER 500


void setup_scheduler(char *sched_name)
{
	tick++;
	//Set up flags for the three schedulers
	int scheduler = 0;
	if (strncmp(sched_name,"FCFS",10) == 0) {
		scheduler = fcfs;
	} else if (strncmp(sched_name,"NPPRI",10) == 0) {
		scheduler = nppri;
	} else if (strncmp(sched_name,"SJF",10) == 0) {
		scheduler = sjf;
	} else {
		printf("Unknown scheduler\n");
		return;
	}

	int running_job = 0;
	task *cur_job;
	int jobs = 0;

	//actual scheduler
	printf("%s",sched_name);
	while (jobs < num_jobs) {
		//Add jobs that have arrived to queue
		add2queue();
		//Sort queue according to scheduler
		if (scheduler == fcfs) {
			qsort(&queue[q_first], q_size, sizeof(task *),fcfs_cmp);
		} else if (scheduler == nppri) {
			qsort(&queue[q_first], q_size, sizeof(task *),nppri_cmp);
		} else if (scheduler == sjf) {
			qsort(&queue[q_first], q_size, sizeof(task *),sjf_cmp);
		}
		//Check if currently running a task else get new task or
		//idle if no tasks in queue
		if (running_job) {
			//check if job has finised
			if (cur_job->burst == tick - cur_job->start_tick) {
				cur_job->end_tick = tick;
				running_job = 0;
				jobs++;
			} else {
				//run job
				printf(",%d",cur_job->pid);
			}
		}
		if (!running_job) {
			if (q_size == 0) {
				//if not last job then idle
				if (jobs < num_jobs) {
					printf(", ");
				}
			} else {
				//get job from queue and run it once
				cur_job = queue[q_first];
				cur_job->start_tick = tick;
				printf(",%d",cur_job->pid);
				//set flags and change size of the queue.
				q_first++;
				running_job = 1;
				q_size--;
			}
		}
		tick++;
	}
	printf("\n");
	finalcount();
}

void add2queue() {
	int i;
	for (i = 0; i < num_jobs; ++i) {
		if (alljobs[i]->arr == tick) {
			queue[q_size+q_first]=alljobs[i];
			q_size++;
		}
	}
}

void finalcount() {
	//sort according to pid
	qsort(alljobs, num_jobs, sizeof(task *),pid_cmp);
	int job;
	//print out all the job stuff
	for (job = 0; job < num_jobs; job++) {
		int resp = alljobs[job]->start_tick-alljobs[job]->arr;
		int turn = alljobs[job]->end_tick-alljobs[job]->arr;
		printf("%d,%d,%d\n",alljobs[job]->pid,resp, turn);
	}
}

char *read_line(char *buffer, FILE *fp)
{
	char *result = fgets(&buffer[0],MAX_BUFFER,fp);
	return result;
}

void read_config(const char *config_file)
{
	char buffer[MAX_BUFFER];
	char *tid,*arr, *burst, *prio;
	FILE *fp = fopen(config_file, "r");
	int i = 0;
	while(read_line(&buffer[0], fp) != NULL) {
		task *t = malloc(sizeof(task));
		if (strnlen(&buffer[0],MAX_BUFFER)<=1) {
			printf("skipping empty line\n");
			continue;
		}
		tid = strtok(buffer,",");
		arr = strtok(NULL,",");
		burst = strtok(NULL,",");
		prio = strtok(NULL,",");
		t->arr = atoi(arr);
		t->pid = atoi(tid);
		t->burst = atoi(burst);
		t->prio = atoi(prio);
		alljobs[i] = t;
		++i;
	}
	num_jobs = i;
	fclose(fp);
	printf("finished reading config file\n");
}
int fcfs_cmp(const void *t1,const void *t2) {
	task **task1 = (task **)t1;
	task **task2 = (task **)t2;
	int arr1 = (*task1)->arr;
	int arr2 = (*task2)->arr;
	if (arr1 < arr2) {
		return -1;
	} else {
		return 1;
	}
}


int nppri_cmp(const void *t1, const void *t2) {
	task **task1 = (task **)t1;
	task **task2 = (task **)t2;
	int p1 = (*task1)->prio;
	int p2 = (*task2)->prio;
	if (p1 < p2) {
		return -1;
	} else {
		return 1;
	}
}

int sjf_cmp(const void *t1, const void *t2) {
	task **task1 = (task **)t1;
	task **task2 = (task **)t2;
	int j1 = (*task1)->burst;
	int j2 = (*task2)->burst;
	if (j1 < j2) {
		return -1;
	} else {
		return 1;
	}
}

int pid_cmp(const void *t1, const void *t2) {
	task **task1 = (task **)t1;
	task **task2 = (task **)t2;
	int p1 = (*task1)->pid;
	int p2 = (*task2)->pid;
	if (p1 < p2) {
		return -1;
	} else {
		return 1;
	}
}

void print_alljobs() {
	int i;
	for (i = 0; i < num_jobs; ++i) {
		printf("pid: %d arr: %d burst: %d prio: %d\n",alljobs[i]->pid,alljobs[i]->arr,alljobs[i]->burst,alljobs[i]->prio);
	}
}

int main(int argc, char **argv)
{
	int opt;
	char schedulerName[256];
	char filename[256];
	int sflag = 0;
	int iflag = 0;
	char *sfile;
	char *ifile;
	while ((opt = getopt(argc, argv, "s:i:")) != -1) {
		switch (opt) {
		case 's':
			sflag = 1;
			sfile = optarg;
			break;
		case 'i':
			iflag = 1;
			ifile = optarg;
			break;
		default:                /* '?' */
			fprintf(stderr, "Usage: -s scheduler -i input_file\n");
			exit(EXIT_FAILURE);
		}
	}
	if (sflag) {
		strncpy(&schedulerName[0], sfile, 256);
	} else {
		fprintf(stderr, "Usage: -s scheduler -i input_file\n");
		exit(EXIT_FAILURE);
	}

	if (iflag) {
		strncpy(&filename[0], ifile, 256);
	} else {
		fprintf(stderr, "Usage: -s scheduler -i input_file\n");
		exit(EXIT_FAILURE);
	}

	alljobs = malloc(1000*sizeof(task*));

	printf("reading config_file %s\n",&filename[0]);
	read_config(&filename[0]);
	//sort according to arrival time
	qsort(alljobs, num_jobs, sizeof(task *),fcfs_cmp);
	//run scheduler 
	setup_scheduler(&schedulerName[0]);
	//free malloced structs
	int i;
	for (i = 0; i < num_jobs; i++) {
		free(alljobs[i]);
	}
	free(alljobs);
	return 0;
}

