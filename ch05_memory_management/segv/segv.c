#include <stdio.h>
#include <stdlib.h>

int main(void)
{
    int *p = NULL;
    printf("before invalid memory access");
    *p = 0;
    printf("after invalid memory access");
    exit(EXIT_SUCCESS);
}