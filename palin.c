/**
 * Author: Taylor Freiner
 * Date: September 25, 2017
 * Log: Code cleanup
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
	int processIndex, stringCount, stringIndex;
	processIndex = atoi(argv[1]);
	stringCount = atoi(argv[2]);
	stringIndex = atoi(argv[3]);
	if(stringIndex == 0)
		stringIndex = 5;

	process(processIndex, stringIndex, stringCount);
	return 0;
}

void process(const int i, int stringIndex, int stringCount){
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
	int n = stringCount;
	int j;
	int idle = 0;
	int want_in = 1;
	int in_cs = 2;
	int exit = 0;
	do {
		do {
			memcpy(&flag[i], &want_in, 4);
			j = *turn;
			while ( j != i )
				j = ( flag[j] != idle ) ? *turn : ( j + 1 ) % n;
			memcpy(&flag[i], &in_cs, 4);
			for ( j = 0; j < n; j++ )
				if ( ( j != i ) && ( flag[j] == 2 ) )
					break;
		} while ( ( j < n ) || ( *turn != i && flag[*turn] != 0) );
		*turn = i;
		for(k = 0; k < stringIndex; k++){
			srand(time(NULL));
			float time1 = (float)rand()/(float)(RAND_MAX/2);
			float time2 = (float)rand()/(float)(RAND_MAX/2);
			sleep(time1);
			palin(stringCount, i, (i*5)+k);
			sleep(time2);
		}
		exit = 1;
		j = (*turn + 1) % n;
		while (flag[j] == 0)
			j = (j + 1) % n;
		*turn = j; memcpy(&flag[i], &idle, 4);
	
	} while (exit == 0);
}

void palin(int stringCount, int processIndex, int stringIndex){
	int i = 0;
	int j = 0;
	FILE *palin_file = fopen("palin.out", "a");
	FILE *non_palin_file = fopen("nopalin.out", "a");
	key_t key = ftok("keygen", 1);
	int memid = shmget(key, stringCount*256, 0);
	char (*mystring)[256] = shmat (memid, NULL, 0);
	for(i = 0; i < stringCount; i++)
		printf(".%s. ", mystring[i]);
	char* string = mystring[stringIndex];
	j = strlen(string) - 1;
	i = 0;
	
	while (i < j){
		if (string[i] != string[j]){
			fprintf(non_palin_file, "%ld ", (long)getpid());
			fprintf(non_palin_file, "%d ", stringIndex);
			fprintf(non_palin_file, "%s\n", string);
			fclose(non_palin_file);
			return;
		}
		i++;
		j--;
	}
	fprintf(palin_file, "%ld ", (long)getpid());
	fprintf(palin_file, "%d ", stringIndex);
	fprintf(palin_file, "%s\n", string);
	fclose(palin_file);
	return;
}
