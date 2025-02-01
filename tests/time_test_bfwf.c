#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int main()
{
	clock_t start, end;
	double cpu_time_used;
  char * ptr3 = ( char * ) malloc ( 200 );
	char * buffer3 = ( char * ) malloc( 1 );
	char * ptr4 = ( char * ) malloc ( 3000 );
  char * buffer2 = ( char * ) malloc( 1 );
  char * ptr1 = ( char * ) malloc ( 500 );
  char * buffer1 = ( char * ) malloc( 1 );
  char * ptr2 = ( char * ) malloc ( 5000 );



  printf("Worst fit should pick this one: %p\n", ptr2 );
  printf("Best fit should pick this one: %p\n", ptr1 );

  free( ptr1 ); 
  free( ptr2 ); 

  buffer1 = buffer1;
  buffer2 = buffer2;
	buffer3 = buffer3;
  ptr4 = ptr4;
	ptr3 = ptr3;

	start = clock();
  char * ptr5 = ( char * ) malloc ( 500 );
	end = clock();
	cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
  printf("Chosen address: %p\n", ptr5 );
	printf("Time taken: %f seconds\n", cpu_time_used);

  return 0;
}

