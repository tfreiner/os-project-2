/**
 * Author: Taylor Freiner
 * Date: September 25, 2017
 * Log: Code cleanup 
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
	for(i = -1; i < processcount; i++){
		kill(processids[processcount], SIGKILL);		
	}
}

int main(int argc, char* argv[]){
	int option, size, i;
	int max_time = 60;
	char argval;
	int count = 0;
	char line[256];
	pid_t childpid = 0;
	//SIGNAL HANDLING
	signal(SIGINT, clean);
	signal(SIGALRM, clean);

	//FILE MANAGEMENT
	FILE *file = fopen("strings.txt", "r");
	
	if (file == NULL){
		printf("%s: ", argv[0]);
		perror ("Error: ");
		return 1;
	}

	size = sizeof(file);
	
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
	
	//OPTIONS
	if (argc != 3){
		fprintf(stderr, "%s Error: Incorrect number of arguments\n", argv[0]);
		return 1;
	}
	while ((option = getopt(argc, argv, "ht:")) != -1){
		switch (option){
			case 'h':
				printf("Usage: %s <-t positive integer>\n", argv[0]);
				printf("\t-t: max time to run program\n");
          			printf("\t-h: help\n");
				return 0;
				break;
			case 't':
				argval = *optarg;
				if(isdigit(argval) && (atoi(optarg) > 0)){
					max_time = atoi(optarg);
					alarm(max_time);
				}
				else{
					fprintf(stderr, "%s Error: Argument must be a positive integer\n", argv[0]);
				}
				break;
			case '?':
				fprintf(stderr, "%s Error: Usage: %s <-t positive integer>\n", argv[0], argv[0]);
				return 1;
				break;
		}
	}
	//SHARED MEMORY
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
	if(*mempoint == -1)
		perror("Error\n");
	if(*mempoint2 == -1)
		perror("Error\n");
	if(*mempoint3 == -1)
		perror("Error\n");
	for(i = 0; i < count; i++){
		memcpy(mempoint[i], mylist[i], 256);
	}
	for(i = 0; i < 20; i++)
		memcpy(&mempoint3[i], &i, 4);
	int turnint = 0;
	memcpy(mempoint2, &turnint, sizeof(int*));

	//PROCESSES
	int processcount = 0;
	int stringindex = 0;
	int stringCount = count;
	i = 0;
	while(count > 0){
		if(processcount < 19){
			if(count < 5)
				stringindex = count % 5;
			childpid = fork();
			processcount++;
			if(childpid > 0){
				processids[processcount] = childpid;
			}
			else if(childpid == 0){
				char arg[12], arg2[12], arg3[12];
				sprintf(arg, "%d", i);
				sprintf(arg2, "%d", stringCount);
				sprintf(arg3, "%d", stringindex);
				execl("palin", "palin", arg, arg2, arg3, NULL);
			}
			else if(childpid == -1){
				printf("%s: ", argv[0]);
				perror("Error:");
			}
		}
		else if(wait(NULL)){
			processcount--;
		}
		count -= 5;
		i++;
	}

	sleep(40);	
	shmctl(memid, IPC_RMID, NULL);
	shmctl(memid2, IPC_RMID, NULL);
	shmctl(memid3, IPC_RMID, NULL);
	return 0;
}
