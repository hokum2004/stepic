#include <iostream>
#include <fstream>
#include <cstring>

#include <errno.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>

/*
Напишите многопоточную программу (POSIX Threads).

Должны быть как минимум:

1. Один поток, ожидающий освобождения мьютекса.
2. Один поток, ожидающий освобождения спин-блокировки.
3. Два потока, ожидающих освобождения RW-блокировки (один должен ожидать чтения, другой - записи).

Текст программы должен находиться в файле /home/box/main.cpp
PID запущенного процесса должен быть сохранен в файле /home/box/main.pid
*/

pthread_spinlock_t spin;

bool isTerminated = false;
void handler_interrupt(int signum)
{
    isTerminated = true;
}

template<typename F, typename R, typename ... Args>
auto sysrun(F* f, R fail_code, Args&& ... args) -> decltype(f(std::forward<Args>(args)...))
{
    auto res = f(std::forward<Args>(args)...);
    if (res == fail_code) {
        perror(__PRETTY_FUNCTION__);
        exit(1);
    }
    return res;
}

void* waiting_mutex(void* arg)
{
    pthread_mutex_t* mutex = reinterpret_cast<pthread_mutex_t*>(arg);

    std::cout << "waiting mutex..." << std::endl;
    int err = pthread_mutex_lock(mutex);
    if (err != 0) {
        char buf[256];
        strerror_r(err, buf, 256);
        std::cerr << "pthread_mutex_lock: " << buf << std::endl;
        pthread_exit(nullptr);
    }

    std::cout << "mutex was locked" << std::endl;
    pthread_mutex_unlock(mutex);

    pthread_exit(nullptr);
}

void* waiting_spinlock(void* arg)
{
    pthread_spinlock_t* spinlock = &spin;

    std::cout << "waiting spinlock..." << std::endl;
    int err =  pthread_spin_lock(spinlock);
    if (err != 0) {
        char buf[256];
        strerror_r(err, buf, 256);
        std::cerr << "pthread_spin_lock: " << buf << std::endl;
        pthread_exit(nullptr);
    }

    std::cout << "spin was locked" << std::endl;
    pthread_spin_unlock(spinlock);

    pthread_exit(nullptr);
}

void* waiting_read_lock(void* arg)
{
    pthread_rwlock_t* rwlock = reinterpret_cast<pthread_rwlock_t*>(arg);

    std::cout << "waiting read lock..." << std::endl;
    int err = pthread_rwlock_rdlock(rwlock);
    if (err != 0) {
        char buf[256];
        strerror_r(err, buf, 256);
        std::cerr << "pthread_rwlock_rdlock: " << buf << std::endl;
        pthread_exit(nullptr);
    }

    std::cout << "rwlock was locked for reading" << std::endl;
    pthread_rwlock_unlock(rwlock);

    pthread_exit(nullptr);
}

void* waiting_write_lock(void* arg)
{
    pthread_rwlock_t* rwlock = reinterpret_cast<pthread_rwlock_t*>(arg);

    std::cout << "waiting write lock..." << std::endl;
    int err = pthread_rwlock_wrlock(rwlock);
    if (err != 0) {
        char buf[256];
        strerror_r(err, buf, 256);
        std::cerr << "pthread_rwlock_wrlock: " << buf << std::endl;
        pthread_exit(nullptr);
    }

    std::cout << "rwlock was locked for writing" << std::endl;
    pthread_rwlock_unlock(rwlock);

    pthread_exit(nullptr);
}

template<typename F, typename ... Args>
auto run(F* f, Args&& ... args) -> decltype(f(std::forward<Args>(args)...))
{
    errno = f(std::forward<Args>(args)...);
    if (errno != 0) {
        perror(__PRETTY_FUNCTION__);
        exit(1);
    }
    return errno;
}

void perror_r(int err, const char* str)
{
    char buf[256];
    strerror_r(err, buf, 256);
    std::cerr << str << ": " << err <<std::endl;
}

template<typename F, typename ... Args>
auto runnoexit(F* f, Args&& ... args) -> decltype(f(std::forward<Args>(args)...))
{
    int err = f(std::forward<Args>(args)...);
    if (err != 0)
        perror_r(err, __PRETTY_FUNCTION__);
    return err;
}

int main(int argc, char * const argv[])
{
    sysrun(&signal, SIG_ERR, SIGINT, &handler_interrupt);

    std::ofstream out("/home/box/main.pid", std::ios::out | std::ios::trunc);
    out << getpid() << std::endl;
    out.close();
    std::cout << "main pid: " << getpid() << std::endl;

    pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
    run(&pthread_spin_init, &spin, PTHREAD_PROCESS_PRIVATE);
    pthread_rwlock_t rwlock = PTHREAD_RWLOCK_INITIALIZER;

    std::cout << "all lock..." << std::endl;
    run(&pthread_mutex_trylock, &mutex);
    run(&pthread_spin_trylock, &spin);
    run(&pthread_rwlock_wrlock, &rwlock);

    std::cout << "run threads..." << std::endl;

    pthread_t t1;
    runnoexit(&pthread_create, &t1, nullptr, &waiting_mutex, &mutex);

    pthread_t t2;
    runnoexit(&pthread_create, &t2, nullptr, &waiting_spinlock, nullptr);

    pthread_t t3;
    runnoexit(&pthread_create, &t3, nullptr, &waiting_read_lock, &rwlock);

    pthread_t t4;
    runnoexit(&pthread_create, &t4, nullptr, &waiting_write_lock, &rwlock);

    while(!isTerminated)
        pause();

    std::cout << "\nall unlock..." << std::endl;
    run(&pthread_mutex_unlock, &mutex);
    run(&pthread_spin_unlock, &spin);
    run(&pthread_rwlock_unlock, &rwlock);

    runnoexit(&pthread_join, t1, nullptr);
    runnoexit(&pthread_join, t2, nullptr);
    runnoexit(&pthread_join, t3, nullptr);
    runnoexit(&pthread_join, t4, nullptr);

    std::cout << "free all resources..." << std::endl;
    runnoexit(&pthread_mutex_destroy, &mutex);
    runnoexit(&pthread_spin_destroy, &spin);
    runnoexit(&pthread_rwlock_destroy, &rwlock);

    std::cout << "finished" << std::endl;

    return 0;
}

