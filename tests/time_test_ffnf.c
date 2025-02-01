#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int main() {
    clock_t start, end;
    double cpu_time_used;

    char *ptr1 = (char *)malloc(4000);
    char *buf1 = (char *)malloc(1);

    char *ptr2 = (char *)malloc(100);
    char *buf2 = (char *)malloc(1);
  
    char *ptr3 = (char *)malloc(5000);
    char *buf3 = (char *)malloc(1);

    char *ptr5 = (char *)malloc(6000);
    char *buf5 = (char *)malloc(1);

    char *ptr4 = (char *)malloc(200);
    char *buf4 = (char *)malloc(1);
  


    char *ptr6 = (char *)malloc(400);

    printf("First fit should pick this one: %p\n", ptr1);
    printf("Next fit should pick this one: %p\n", ptr4);

    free(ptr1);
    free(ptr3);
    free(ptr4);
    char *ptr9 = (char *)malloc(5000);
    ptr9 = ptr9;
      ptr2 = ptr2;
    ptr5 = ptr5;
    ptr6 = ptr6;
    //ptr7 = ptr7;
    buf1 = buf1;
    buf2 = buf2;
    buf3 = buf3;
    buf4 = buf4;
    buf5 = buf5;

    // Simulate a large memory allocation operation
    start = clock();
    char *ptr8 = (char *)malloc(1000); // Adjust size for testing
    end = clock();
    cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;

    printf("Chosen address: %p\n", ptr8);
    printf("Time taken: %f seconds\n", cpu_time_used);

    return 0;
}
