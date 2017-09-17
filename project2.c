/*
 * Author: Taylor Freiner
 * Date: September 16, 2017
 * Log: Adding help option
 */
#include <stdio.h>
#include <getopt.h>

int main(int argc, char* argv[]){
	int option;
	if (argc != 1 && argc != 2){
		fprintf(stderr, "%s Error: Incorrect number of arguments\n", argv[0]);
		return 1;
	}
	while ((option = getopt(argc, argv, "h")) != -1){
		switch (option){
			case 'h':
				printf("ADD USAGE STATEMENT %s\n", argv[0]);
          			printf("\t-h: help\n");
				return 0;
				break;
			case '?':
				fprintf(stderr, "%s Error: ADD USAGE STATEMENT %s\n", argv[0], argv[0]);
				return 1;
				break;
		}
	}
	return 0;
}

//PETERSON'S SOLUTION
/*
extern int flag[2]; // Shared variable; one for each process
extern int turn; //Shared variable
void process ( const int me ) { // me can be 0 or 1
	int other = 1 - me;
	do{
	// Entry section
		flag[me] = 1; // true
		turn = other   // cede the turn
		while ( flag[other]  && turn == other);
		critical_section();
		flag[me] = 0; // false
		remainder_section();
	} while ( 1 );
}
*/

//MULTIPLE PROCESS SOLUTION
/*
enum state { idle, want_in, in_cs };
extern int turn;
extern state flag[n]; //Flag corresponding to each process in shared memory
process(const int i ) {
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
		critical_section();
		// Exit section
		j = (turn + 1) % n;
		while (flag[j] == idle)
			j = (j + 1) % n;
		// Assign turn to next waiting process; change own flag to idle
		turn = j; flag[i] = idle;
		remainder_section();
	} while ( 1 );
}
*/

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
