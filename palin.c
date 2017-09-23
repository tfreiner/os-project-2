#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <sys/shm.h>
#include <errno.h>

int main(int argc, char* argv[]){	
	int i;
	printf("IN PALIN\n");
	
	key_t key = ftok("dummyfile", 1);
	//printf("%s\n", argv[1]);
	int memid = shmget(key, 8*256, 0);
	printf("MEM ID: .%d.\n", memid);
	char (*mempoint)[7][256] = shmat (memid, (void *)0, 0);
	printf("FIRST STRING: %s\n", mempoint[1]);
	//sleep(100);
	//if(mempoint == (char**)-1){
	//	printf("ERROR");
	//}
	//printf("ADDRESS IN PALIN: %c\n", mempoint[0][0]);
	
	for(i = 0; i < 5; i++){
		srand(time(NULL));
		float time1 = (float)rand()/(float)(RAND_MAX/2);
		float time2 = (float)rand()/(float)(RAND_MAX/2);
		//execute code to enter critical section;
		/* Critical section */
	//	sleep(time1);
		//write message into the file
	//	sleep(time2);
		//execute code to exit from critical section;
	}
	FILE *palin_file = fopen("palin.out", "w");
	fprintf(palin_file, "here");
	fclose(palin_file);
	return 0;
}
