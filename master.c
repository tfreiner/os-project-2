/**
 * Author: Taylor Freiner
 * Date: September 23, 2017
 * Log: Fixed shared mem and while loop
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

void clean(){
	printf("Interrupt Triggered\n");
	int i;
	printf("Mem Count: %d\n", memcount);
	for(i = -1; i < memcount; i++){
		printf("Exit Id: %d\n", sharedmem[memcount]);
		shmctl(sharedmem[memcount], IPC_RMID, NULL);
	}
	for(i = -1; i < processcount; i++){
		printf("Killing Process: %d\n", processids[processcount]);
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
					printf("Max Time: %d\n", max_time);
				}
				else{
					fprintf(stderr, "%s Error: Argument must be a positive integer\n", argv[0]);
				}
				break;
			case '?':
				fprintf(stderr, "%s Error: ADD USAGE STATEMENT %s\n", argv[0], argv[0]);
				return 1;
				break;
		}
	}
	
	//SHARED MEMORY
	key_t key = ftok("keygen", 1);
	int memid = shmget(key, count*256, IPC_CREAT | 0644);
	printf("Initial ID: %d\n", memid);
	memcount++;
	sharedmem[memcount] = memid;
	if(memid == -1){
		printf("%s: ", argv[0]);
		perror("Error:");
		return 1;
	}
	char (*mempoint)[count] = shmat (memid, NULL, 0);
	if(mempoint == (char *)-1)
		printf("ERROR\n");
	for(i = 0; i < count; i++){
		memcpy(mempoint[i], mylist[i], 256);
		printf(".%s. ", mempoint[i]);
	}
	printf("\n");

	//PROCESSES
	int processcount = 0;
	i = 0;
	while(i != count){
		printf("PROCESS COUNT: %d\n", processcount);
		if(processcount < 19){
			childpid = fork();
			processcount++;
			
			if(childpid > 0){
				printf("Parent Process\n");
				printf("%d\n", childpid);
				processids[processcount] = childpid;
			}
			else if(childpid == 0){
				printf("Child Process\n");
				char arg[12], arg2[12];
				sprintf(arg, "%d", i);
				sprintf(arg2, "%d", count);
				execl("palin", "palin", arg, arg2, NULL);
				break;
			}
			else if(childpid == -1){
				printf("%s: ", argv[0]);
				perror("Error:");
			}
		}
		else if(wait(NULL)){
			processcount--;
			printf("DECREMENT COUNTER\n");
		}
		i++;
	}
	
	printf("Out of loop\n");
	sleep(20);
	shmctl(memid, IPC_RMID, NULL);
	return 0;
}
