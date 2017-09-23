/**
 * Author: Taylor Freiner
 * Date: September 23, 2017
 * Log: Retrieving data from shared mem
 */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <sys/shm.h>
#include <errno.h>
#include <string.h>

void process(const int, int);
void palin(int, int);

int main(int argc, char* argv[]){	
	int i, j, count;
	printf("In Palin\n");
	printf("%s\n", argv[2]);
	j = atoi(argv[1]);
	count = atoi(argv[2]);
	//key_t key = ftok("dummyfile", 1);
	//printf("%s\n", argv[1]);
	//int memid = shmget(key, 8*256, 0);
	//printf("Mem Id: %d\n", memid);
	//char (*mempoint)[count][256] = shmat (memid, (void *)0, 0);
	//printf("String: %s\n", mempoint[1][0]);

	//process(j, count);
	palin(count, j);
/*	
	for(i = 0; i < 5; i++){
		srand(time(NULL));
		float time1 = (float)rand()/(float)(RAND_MAX/2);
		float time2 = (float)rand()/(float)(RAND_MAX/2);
		sleep(time1);
		process(j, count);
		sleep(time2);
	}
	FILE *palin_file = fopen("palin.out", "w");
	fprintf(palin_file, "here");
	fclose(palin_file);
*/
	return 0;
}

void process(const int i, int count){
	enum state {idle, want_in, in_cs};
	int turn;
	int n = 20;
	enum state flag[n]; //Flag corresponding to each process in shared memory

	int j;
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
		palin(count, i);
		// Exit section
		j = (turn + 1) % n;
		while (flag[j] == idle)
			j = (j + 1) % n;
		// Assign turn to next waiting process; change own flag to idle
		turn = j; flag[i] = idle;
		//remainder_section();
	} while ( 1 );
}

void palin(int count, int index){
	int i = 0;
	int j = 0;
	char charindex[12];
	sprintf(charindex, "%d", index);	
	FILE *palin_file = fopen("palin.out", "a");
	FILE *non_palin_file = fopen("nopalin.out", "a");
	printf("COUNT: %d\n", count);
	key_t key = ftok("keygen", 1);
	int memid = shmget(key, count*256, 0);
	char (*mystring)[count] = shmat (memid, NULL, 0);
	for(i = 0; i < count; i++)
		printf(".%s. ", mystring[i]);
	printf("\n");
	char* string = mystring[index];
	j = strlen(string) - 1;
	i = 0;
	printf("I: %d J: %d\n", i, j);
	
	while (i < j){
		if (string[i] != string[j]){
			printf("%s is not a palindrome\n", string);
			fprintf(non_palin_file, charindex);
			fprintf(non_palin_file, "\n");
			fclose(non_palin_file);
			return;
		}
		i++;
		j--;
	}
	printf("%s is a palindrome\n", string);
	fprintf(palin_file, charindex);
	fprintf(palin_file, "\n");
	fclose(palin_file);
	return;

}
