#include <iostream>
#include <cstdlib>
#include <cstdio>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <signal.h>
#include <unistd.h>

bool is_terminated = false;
void handler(int)
{
    is_terminated = true;
}

union semun {
    int val;
    struct semid_ds* buf;
    unsigned short* array;
    struct seminfo* __buf;
};

int main(int argc, char * const argv[])
{
    const char key_path[] = "/tmp/sem.temp";
    const int proj_id = 1;
    unsigned short sem_count = 16;

    if (signal(SIGINT, &handler) == SIG_ERR) {
        perror("signal");
        exit(1);
    }

    key_t semkey = ftok(key_path, proj_id);
    if (semkey == -1) {
        perror("ftok");
        exit(2);
    }

    int semid = semget(semkey, sem_count, IPC_CREAT);
    if (semid == -1) {
        perror("semget");
        exit(3);
    }

    semun param = {0};
    semid_ds sem_data = {0};
    sem_data.sem_perm.uid = geteuid();
    sem_data.sem_perm.gid = getegid();
    sem_data.sem_perm.mode = 0666;
    param.buf = &sem_data;

    if (semctl(semid, 0, IPC_SET, param) == -1)
        perror("semctl IPC_SET");

    for (unsigned short sem_num = 0; sem_num < sem_count; ++sem_num) {
        param.val = sem_num;
        if (semctl(semid, sem_num, SETVAL, param) == -1) {
            perror("semctl SETVAL");
            std::cerr << "sem_num: " << sem_num << std::endl;
            exit(5);
        }
    }

#if 1
    {
        semun info = {0};
        unsigned short sem_vals[sem_count];
        info.array = sem_vals;

        if (semctl(semid, 0, GETALL, info) == -1)
            perror("semctl GETALL");

        for (auto v: sem_vals)
            std::cout << v << " ";
        std::cout << std::endl;
    }
#endif

    while(!is_terminated)
        pause();

    if (semctl(semid, 0, IPC_RMID) == -1) {
        perror("semctl");
        exit(10);
    }

    return 0;
}
