#include <iostream>
#include <fstream>

#include <errno.h>
#include <unistd.h>
#include <pthread.h>

void* worker_func(void*)
{
    std::cout << "thread pid: " << getpid() << std::endl;
    for(int i = 10; i; --i) {
        std::cout << i << std::endl;
        sleep(1);
    }

    pthread_exit(nullptr);
}

int main(int argc, char * const argv[])
{
    std::ofstream out("/home/box/main.pid", std::ios::out | std::ios::trunc);
    out << getpid() << std::endl;
    out.close();
    std::cout << "main pid: " << getpid() << std::endl;

    pthread_t worker_id;
    errno = pthread_create(&worker_id, nullptr, &worker_func, nullptr);
    if (errno != 0) {
        perror("pthread_create");
        exit(1);
    }
    std::cout << "worker was started" << std::endl;

    errno = pthread_join(worker_id, nullptr);
    if (errno != 0) {
        perror("pthread_join");
        exit(1);
    }

    std::cout << "finished" << std::endl;

    return 0;
}

