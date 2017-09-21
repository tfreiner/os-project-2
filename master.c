/*
 * Author: Taylor Freiner
 * Date: September 20, 2017
 * Log: Adding shared memory management
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
#include <sys/shm.h>
#include "palin.h"

void clean(){
	printf("INTERRUPT TRIGGERED\n");
}

int main(int argc, char* argv[]){
	int option, size, i;
	int max_time = 60;
	char argval;
	int count = 0;
	char line[256];

	//SIGNAL HANDLING
	signal(SIGINT, clean);

	palin();
	
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
	char *mylist[count];
	for(i = 0; i < count; i++){
		fgets(line, sizeof(line), file);
		size_t line_size = strlen(line)-1;
		if (line[line_size] == '\n')
    			line[line_size] = '\0';
		mylist[i] = line;
		printf(".%s.\n", mylist[i]);
	}
	fclose(file);

	/*
	time_t startTime = time(NULL);
	do
	{
		printf("HEY\n");
	} while (time(NULL) < startTime + max_time);
	return 0;
	sleep(10);	
         */

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
				if(isdigit(argval) && (atoi(optarg) > 0))
					max_time = atoi(optarg);
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
	int memid = shmget(IPC_PRIVATE, size, IPC_CREAT | 0600);	
	if(memid == -1){
		printf("%s: ", argv[0]);
		perror("Error:");
		return 1;
	}
	shmctl(memid, IPC_RMID, NULL);
	return 0;
}

process(const int i ) {
	enum state { idle, want_in, in_cs };
	int turn;
	enum state flag[20]; //Flag corresponding to each process in shared memory
	
	int j, n;
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

//SIGNAL HANDLERS
/*
void myhandler(int signo) {
	int esaved;
	esaved = errno;
	write(STDOUT_FILENO, "Got a signal\n", 13);
	errno = esaved;
}

#include <setjmp.h>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>
static sigjmp_buf jmpbuf;
static volatile sig_atomic_t jumpok = 0;
// ARGSUSED
static void chandler(int signo) {
	if (jumpok == 0) return;
	siglongjmp(jmpbuf, 1);
}
int main(void)  {
	struct sigaction act;
	act.sa_flags = 0;
	act.sa_handler = chandler;
	if ((sigemptyset(&act.sa_mask) == -1) ||
		(sigaction(SIGINT, &act, NULL) == -1)) {
		perror("Failed to set up SIGINT handler");
		return 1;
	}
	// stuff goes here
	fprintf(stderr, "This is process %ld\n", (long)getpid());
	if (sigsetjmp(jmpbuf, 1))
		fprintf(stderr, "Returned to main loop due to ^c\n");
	jumpok = 1;
	for ( ; ; )
		// main loop goes here
}
*/
