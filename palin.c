/**
 * Author: Taylor Freiner
 * Date: September 25, 2017
 * Log: More work on critical section
 */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <sys/shm.h>
#include <errno.h>
#include <string.h>

void process(const int, int, int);
void palin(int, int, int);

int main(int argc, char* argv[]){	
	printf("----------------------IN MAIN PALIN FILE-------------------------------\n");
	int i, processIndex, stringCount, stringIndex;
	processIndex = atoi(argv[1]);
	printf("PROCESS INDEX: %d\n", processIndex);
	stringCount = atoi(argv[2]);
	//printf("STRING COUNT: %d\n", stringCount);
	stringIndex = atoi(argv[3]);
	if(stringIndex == 0)
		stringIndex = 5;
//	palin(stringCount, stringIndex);
//	process(processIndex, stringIndex, stringCount);

/*
	for(i = 0; i < stringIndex; i++){
		printf("-----------------------FOR LOOP-------------------------\n%d", processIndex);
		srand(time(NULL));
		float time1 = (float)rand()/(float)(RAND_MAX/2);
		float time2 = (float)rand()/(float)(RAND_MAX/2);
		sleep(time1);
		//palin(stringCount, processIndex, (processIndex*5) + i);
		process(processIndex, (processIndex*5) + i, stringCount);
		printf("LEAVING CRITICAL SECTION\n");
		sleep(time2);
	}
*/
	process(processIndex, stringIndex, stringCount);
	return 0;
}

void process(const int i, int stringindex, int count){
	printf("------------------IN PROCESS-------------------------\n");
	printf("\n\n\nPROCESS INDEX: %d\n\n\n", i);
	printf("\n\n\nSTRING INDEX: %d\n\n\n", stringindex);	
	key_t key2 = ftok("keygen2", 1);
	key_t key3 = ftok("keygen3", 1);
	int memid2 = shmget(key2, sizeof(int*), 0);
	int memid3 = shmget(key3, sizeof(int*)*19, 0);
	int *turn = (int *)shmat (memid2, NULL, 0);
	int *flag = (int *)shmat (memid3, NULL, 0);
	int flagvar = -1;
	int k;
	for(k = 0; k < 20; k++)
		 memcpy(&flag[k], &flagvar, 4);
	int n = count;
	int j;
	int idle = 0;
	int want_in = 1;
	int in_cs = 2;
	int exit = 0;
	do {
		do {
			memcpy(&flag[i], &want_in, 4); // Raise my flag
			j = *turn; // Set local variable
			// wait until its my turn
			while ( j != i )
				j = ( flag[j] != idle ) ? *turn : ( j + 1 ) % n;
			// Declare intention to enter critical section
			memcpy(&flag[i], &in_cs, 4);
			// Check that no one else is in critical section
			for ( j = 0; j < n; j++ )
				if ( ( j != i ) && ( flag[j] == 2 ) )
					break;
		} while ( ( j < n ) || ( *turn != i && flag[*turn] != 0) );
		// Assign turn to self and enter critical section
		*turn = i;
		for(k = 0; k < stringindex; k++){
			printf("-----------------------FOR LOOP-------------------------\n%d", i);
			srand(time(NULL));
			float time1 = (float)rand()/(float)(RAND_MAX/2);
			float time2 = (float)rand()/(float)(RAND_MAX/2);
			sleep(time1);
			//palin(count, processIndex, (processIndex*5) + i);
			palin(count, i, (i*5)+k);
			sleep(time2);
		}
		//palin(count, i, stringindex);
		exit = 1;
		j = (*turn + 1) % n;
		while (flag[j] == 0)
			j = (j + 1) % n;
		// Assign turn to next waiting process; change own flag to idle
		*turn = j; memcpy(&flag[i], &idle, 4);
	
	} while (exit == 0);
}

void palin(int count, int processIndex, int stringIndex){
	printf("------------------IN PALIN---------------------------\n");
	int i = 0;
	int j = 0;
	//printf("COUNT: %d\n\n", count);
	//printf("INDEX: %d\n\n", processIndex);
	//printf("STRING INDEX: %d\n\n", stringIndex);
//	char charindex[12];
//	sprintf(charindex, "%d", index);	
	FILE *palin_file = fopen("palin.out", "a");
	FILE *non_palin_file = fopen("nopalin.out", "a");
	key_t key = ftok("keygen", 1);
	int memid = shmget(key, count*256, 0);
	char (*mystring)[256] = shmat (memid, NULL, 0);
	for(i = 0; i < count; i++)
		printf(".%s. ", mystring[i]);
	printf("\n");
	char* string = mystring[stringIndex];
	j = strlen(string) - 1;
	i = 0;
	
	while (i < j){
		if (string[i] != string[j]){
			fprintf(non_palin_file, string);
			fprintf(non_palin_file, "\n");
			fclose(non_palin_file);
			return;
		}
		i++;
		j--;
	}
	fprintf(palin_file, string);
	fprintf(palin_file, "\n");
	fclose(palin_file);
	return;
}
