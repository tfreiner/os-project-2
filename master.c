/*
 * Author: Taylor Freiner
 * Date: September 23, 2017
 * Log: More shared memory management
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
	//printf("INTERRUPT TRIGGERED\n");
	int i;
	printf("MEM COUNT: %d\n", memcount);
	for(i = -1; i < memcount; i++){
		printf("EXIT ID: %d\n", sharedmem[memcount]);
		shmctl(sharedmem[memcount], IPC_RMID, NULL);
	}
	for(i = -1; i < processcount; i++){
		printf("KILLING PROCESS: %d\n", processids[processcount]);
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
	//char b[count][256];
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
	key_t key = ftok("dummyfile", 1);
	int memid = shmget(key, 8*256, IPC_CREAT | 0644);
	printf("INITIAL ID: .%d.\n", memid);
	memcount++;
	sharedmem[memcount] = memid;
	if(memid == -1){
		printf("%s: ", argv[0]);
		perror("Error:");
		return 1;
	}
	char (*mempoint)[count][256] = shmat (memid, NULL, 0);
	if(mempoint == (char *)-1)
		printf("ERROR\n");
	for(i = 0; i < count; i++){
		memcpy(mempoint[i], mylist[i], 256);
	}
	printf("%s\n", mempoint[0][0]);
	printf("%s\n", mempoint[1][0]);
	//PROCESSES
	for(i = 0; i < 1; i++){
		if((childpid = fork())){
			processcount++;
			processids[processcount] = childpid;
			execl("palin", "palin", NULL);
			break;
		}
		if(childpid == -1){
			printf("%s: ", argv[0]);
			perror("Error:");
		}
	}
	wait(NULL);

	//shmctl(memid, IPC_RMID, NULL);
	sleep(100);
	return 0;
}

void process(const int i ) {
	enum state { idle, want_in, in_cs };
	int turn;
	enum state flag[20]; //Flag corresponding to each process in shared memory
	
	int j;
	int n = 20;
	do {
		do {
			flag[i] = want_in; // Raise my flag
			j = turn; // Set local variable
			// wait until its my turn
			while ( j != i )
				j = ( flag[j] != idle ) ? turn : ( j + 1 ) % n;
			// Declare intention to enter critical section
			flag[i] = in_cs;
			// Check that no one else is in critical section
			for ( j = 0; j < n; j++ )
				if ( ( j != i ) && ( flag[j] == in_cs ) )
					break;
		} while ( ( j < n ) || ( turn != i && flag[turn] != idle ) );
		// Assign turn to self and enter critical section
		turn = i;
		//critical_section();
		// Exit section
		j = (turn + 1) % n;
		while (flag[j] == idle)
			j = (j + 1) % n;
		// Assign turn to next waiting process; change own flag to idle
		turn = j; flag[i] = idle;
		//remainder_section();
	} while ( 1 );
}
