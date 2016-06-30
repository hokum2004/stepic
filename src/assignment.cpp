#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <cstring>

#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>

bool is_terminated = false;
void handler(int)
{
    is_terminated = true;
}

int main(int argc, char * const argv[])
{
    const char name[] = "/test.shm";
    size_t size = 1024 * 1024;

    if (signal(SIGINT, &handler) == SIG_ERR) {
        perror("signal");
        exit(1);
    }

    int shmd = shm_open(name, O_CREAT | O_RDWR, 0666);
    if (shmd == -1) {
        perror("shm_open");
        exit(1);
    }

    if (ftruncate(shmd, size) == -1) {
        perror("ftruncate");
        exit(1);
    }

    void* shm = mmap(nullptr, size, PROT_WRITE, MAP_SHARED, shmd, 0);
    if (shm == (void *) -1) {
        perror("mmap");
        exit(1);
    }

    memset(shm, 13, size);

    while(!is_terminated)
        pause();

    if (shm_unlink(name) == -1)
        perror("shm_unlink");

    if (munmap(shm, size) == -1)
        perror("munmap");

    return 0;
}
