/**
 * Author: Taylor Freiner
 * Date: October 3, 2017
 * Log: Final touches.
 */
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <signal.h>
#include <setjmp.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/shm.h>

int memcount = -1;
int processcount = -1;
int sharedmem[100];
int processids[100];

void clean(int sig){
	if(sig == 2)
		fprintf(stderr, "Interrupt signaled. Removing shared memory and killing processes.\n");
	else if(sig == 14)
		fprintf(stderr, "Program timed out. Removing shared memory and killing processes.\n");	
	int i;	
	for(i = 0; i < 3; i++){
		shmctl(sharedmem[memcount], IPC_RMID, NULL);
	}
	for(i = 0; i < processcount; i++){
		printf("Killing process %d", processids[i]);
		kill(processids[i], SIGKILL);
	}
}

int main(int argc, char* argv[]){
	int option, i;
	int max_time = 60;
	char argval;
	char *filename = (char *)malloc(100);
	int count = 0;
	char line[256];
	pid_t childpid = 0;

	//SIGNAL HANDLING
	signal(SIGINT, clean);
	signal(SIGALRM, clean);

	if (argc != 5 && argc != 2){
		fprintf(stderr, "%s Error: Incorrect number of arguments\n", argv[0]);
		return 1;
	}
	while ((option = getopt(argc, argv, "ht:f:")) != -1){
		switch (option){
			case 'h':
				printf("Usage: %s <-t positive integer> <-f filename>\n", argv[0]);
				printf("\t-t: max time to run program\n");
				printf("\t-f: name of file containing strings\n");
				printf("\t-h: help\n");
				return 0;
				break;
			case 't':
				argval = *optarg;
				if(isdigit(argval) && (atoi(optarg) > 0)){
					max_time = atoi(optarg);
					alarm(max_time);
				} else {
					fprintf(stderr, "%s Error: Argument must be a positive integer\n", argv[0]);			
				}
				break;
			case 'f':
				filename = optarg;
				break;
			case '?':
				fprintf(stderr, "%s Error: Usage: %s <-t positive integer> <-f filename>\n", argv[0], argv[0]);
				return 1;
				break;
		}
	}
			
	//FILE MANAGEMENT
	FILE *file = fopen(filename, "r");
	
	if (file == NULL){
		printf("%s: ", argv[0]);
		perror ("Error: ");
		return 1;
	}

    	while (fgets(line, sizeof(line), file) != NULL)
    	{
            count++;
	}
	rewind(file);
	char mylist[count][256];
	for(i = 0; i < count; i++){
		fgets(line, sizeof(line), file);
		size_t line_size = strlen(line)-1;
		if (line[line_size] == '\n')
    			line[line_size] = '\0';
		strcpy(mylist[i], line);
	}
	fclose(file);
	
	//SHARED MEMORY
	pid_t index[19];
	for(i = 0; i < 19; i++)
		index[i] = -1;
	key_t key = ftok("keygen", 1);
	key_t key2 = ftok("keygen2", 1);
	key_t key3 = ftok("keygen3", 1);
	int memid = shmget(key, count*256, IPC_CREAT | 0644);
	int memid2 = shmget(key2, sizeof(int*), IPC_CREAT | 0644);
	int memid3 = shmget(key3, sizeof(int*)*19, IPC_CREAT | 0644);
	sharedmem[0] = memid;
	sharedmem[1] = memid2;
	sharedmem[2] = memid3;
	if(memid == -1){
		printf("%s: ", argv[0]);
		perror("Error:");
		return 1;
	}
	char (*mempoint)[256] = shmat (memid, NULL, 0);
	int *mempoint2 = (int *)shmat (memid2, NULL, 0);
	int *mempoint3 = (int *)shmat (memid3, NULL, 0);
	if(*mempoint == (char *) -1)
		perror("Error\n");
	if(*mempoint2 == -1)
		perror("Error\n");
	if(*mempoint3 == -1)
		perror("Error\n");
	for(i = 0; i < count; i++){
		memcpy(mempoint[i], mylist[i], 256);
	}
	int flag = -1;
	for(i = 0; i < 19; i++)
		memcpy(&mempoint3[i], &flag, 4);
	int turnint = 0;
	memcpy(mempoint2, &turnint, sizeof(int));

	//PROCESSES
	int processcount = 0;
	int stringindex = 0;
	int stringCount = count;
	int j;
	int k = -1;
	pid_t processid;
	i = 0;
	while(count > 0){
		if(processcount < 19){
			if(count < 5)
				stringindex = count % 5;
			for(j = 0; j < 19; j++){
				if(index[j] == -1){
					processcount++;
					childpid = fork();
					if(childpid){
						index[j] = childpid;
						i = j;
					}
					k++;
					j = 20;
				}
			}
			if(childpid > 0){
				processids[processcount] = childpid;
			}
			else if(childpid == 0){
				char arg[12], arg2[12], arg3[12], arg4[12];
				sprintf(arg, "%d", i);
				sprintf(arg2, "%d", stringCount);
				sprintf(arg3, "%d", stringindex);
				sprintf(arg4, "%d", k);
				execl("palin", "palin", arg, arg2, arg3, arg4, NULL);
			}
			if(childpid == -1){
				printf("%s: ", argv[0]);
				perror("Error:");
			}
			count -= 5;
		}
		else{
			processid = (wait(NULL));
			processcount--;
			for(j = 0; j < 19; j++){
				if(index[j] == processid){
					index[j] = -1;
					j = 20;
					kill(processid, SIGKILL);
				}
			}
		}
	}
	sleep(10);
	shmctl(memid, IPC_RMID, NULL);
	shmctl(memid2, IPC_RMID, NULL);
	shmctl(memid3, IPC_RMID, NULL);
	return 0;
}
