#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <cstring>

#include <sys/ipc.h>
#include <sys/shm.h>
#include <signal.h>
#include <unistd.h>

bool is_terminated = false;
void handler(int)
{
    is_terminated = true;
}

int main(int argc, char * const argv[])
{
    const char key_path[] = "/tmp/mem.temp";
    const int proj_id = 1;
    const size_t size = 1024 * 1024;

    if (signal(SIGINT, &handler) == SIG_ERR) {
        perror("signal");
        exit(1);
    }

    key_t shm_key = ftok(key_path, proj_id);
    if (shm_key == -1) {
        perror("ftok");
        exit(1);
    }

    int shmid = shmget(shm_key, size, IPC_CREAT | 0666);
    if (shmid == -1) {
        perror("shmget");
        exit(1);
    }

    void * shm = shmat(shmid, nullptr, 0);
    if (shm == (void *) -1) {
        perror("shmat");
        exit(1);
    }

    memset(shm, 42, size);

    while(!is_terminated)
        pause();

    if (shmdt(shm) == -1)
        perror("shmdt");

    if (shmctl(shmid, IPC_RMID, nullptr) == -1)
        perror("shmctl IPCRMID");

    return 0;
}
