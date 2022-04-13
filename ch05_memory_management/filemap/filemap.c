#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <err.h>

#define BUFFER_SIZE 1000
#define ALLOC_SIZE (1000 * 1024 * 1024)

static char command[BUFFER_SIZE];
static char file_contents[BUFFER_SIZE];
static char overwrite_data[] = "HELLO";

int main(void)
{
    pid_t pid;
    pid = getpid();
    snprintf(command, BUFFER_SIZE, "cat /proc/%d/maps", pid);

    printf("\n=== memory map before file mapping ===\n");
    fflush(stdout);
    system(command);
    printf("======================================\n");

    // file mapping
    int fd;
    fd = open("testfile", O_RDWR);
    if (fd == -1)
        err(EXIT_FAILURE, "failed to open testfile");
    
    char * file_contents;
    file_contents = mmap(NULL, ALLOC_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (file_contents == (void *) -1){
        err(EXIT_FAILURE, "failed to mmap");
        goto close_file;
    }

    printf("\nsucceeded to map file: address = 0x%p; size = 0x%x\n", file_contents, ALLOC_SIZE);

    printf("\n=== memory map afrer file mapping ===\n");
    fflush(stdout);
    system(command);
    printf("======================================\n");

    printf("\n=== file contents before overwrite mapped region : %s ===\n", file_contents);
    // overwrite mapped region
    memcpy(file_contents, overwrite_data, strlen(overwrite_data));
    printf("=== file contents after overwrite mapped region : %s ===\n", file_contents);

    if (munmap(file_contents, ALLOC_SIZE) == -1)
        err(EXIT_FAILURE, "failed to munmap");

    close_file:
        if (close(fd) == -1)
            warn("failed to close");

    exit(EXIT_SUCCESS);
}