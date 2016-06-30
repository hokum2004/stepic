#include <iostream>
#include <cstdlib>
#include <cstdio>

#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <semaphore.h>

bool is_terminated = false;
void handler(int)
{
    is_terminated = true;
}

int main(int argc, char * const argv[])
{
    const char name[] = "/test.sem";
    unsigned short init_value = 66;

    if (signal(SIGINT, &handler) == SIG_ERR) {
        perror("signal");
        exit(1);
    }

    sem_t* sem = sem_open(name, O_CREAT, 0666, init_value);
    if (sem == SEM_FAILED) {
        perror("sem_open");
        exit(1);
    }

    int value;
    if (sem_getvalue(sem, &value) == -1)
        perror("sem_getvalue");
    else
        std::cout << "The semaphore value is " << value << std::endl;

    while(!is_terminated)
        pause();

    if (sem_unlink(name) == -1)
        perror("sem_unlink");

    if (sem_close(sem) == -1)
        perror("sem_close");

    return 0;
}
