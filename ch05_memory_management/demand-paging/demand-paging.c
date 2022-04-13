#include <unistd.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <err.h>

#define BUFFER_SIZE (100 * 1024 * 1024)
#define NCYCLE 10
#define PAGE_SIZE (10 * 1024 * 1024)

int main(void)
{
    char *p;
    time_t t;
    char *s;

    t = time(NULL);
    s = ctime(&t);

    printf(
        "%.*s: before allocation, please press Enter key\n",
        (int)(strlen(s) - 1), s
    );
    getchar();

    // allocation
    p = malloc(BUFFER_SIZE);
    if (p == NULL)
        err(EXIT_FAILURE, "failed to malloc p");
    t = time(NULL);
    s = ctime(&t);
    printf(
        "%.*s: allocated %d MB, please press Enter key\n",
        (int)(strlen(s) - 1), s, BUFFER_SIZE / 1024 / 1024
    );
    getchar();

    // output result
    int i;
    for (i = 0; i < BUFFER_SIZE; i += PAGE_SIZE)
    {
        p[i] = 0;
        int cycle = i / (BUFFER_SIZE / NCYCLE);
        if (cycle != 0 && i % (BUFFER_SIZE / NCYCLE)) {
            t = time(NULL);
            s = ctime(&t);
            printf(
                "%.*s: touched %d MB\n",
                (int)(strlen(s) - 1), s, i / (1024 * 1024)
            );
            sleep(1);
        }
    }

    printf(
        "%.*s: before allocation, please press Enter key\n",
        (int)(strlen(s) - 1), s
    );
    getchar();
    exit(EXIT_SUCCESS);
}