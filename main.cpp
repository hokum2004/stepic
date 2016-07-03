#include <iostream>
#include <fstream>
#include <cstring>

#include <errno.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>

bool isTerminated;
void handler_interrupt(int)
{
    isTerminated = true;
}

void perror_r(int err, const char* str)
{
    char buf[512];
    strerror_r(err, buf, 512);
    std::cerr << str << ": " << err <<std::endl;
}


void prepare()
{
    if (signal(SIGINT, &handler_interrupt) == SIG_ERR) {
        perror("signal");
        exit(1);
    }

    std::ofstream out("/home/box/main.pid", std::ios::out | std::ios::trunc);
    out << getpid() << std::endl;
    out.close();
    std::cout << "main pid: " << getpid() << std::endl;
}

template<typename F, typename ... Args>
auto run_pth_fam(F* f, Args&& ... args) -> decltype(f(std::forward<Args>(args)...))
{
    auto err = f(std::forward<Args>(args)...);
    if (err != 0)
        perror_r(err, __PRETTY_FUNCTION__);
    return err;
}

typedef std::pair<pthread_cond_t*, pthread_mutex_t*> cond_pair_t;

void* waiting_cond(void* arg)
{
    cond_pair_t* cond_pair = reinterpret_cast<cond_pair_t*>(arg);
    std::cout << "waiting cond..." << std::endl;
    run_pth_fam(&pthread_cond_wait, cond_pair->first, cond_pair->second);
    std::cout << "unlock1: " << run_pth_fam(&pthread_mutex_unlock, cond_pair->second) << std::endl;;
    sleep(2);
    std::cout << "waiting cond was finished" << std::endl;
    return nullptr;
}

void* waiting_barrier(void* arg)
{
    pthread_barrier_t* barrier = reinterpret_cast<pthread_barrier_t*>(arg);
    std::cout << "waiting barrier..." << std::endl;
    run_pth_fam(&pthread_barrier_wait, barrier);
    std::cout << "waiting barrier was finished" << std::endl;
    return nullptr;
}

int main(int argc, char * const argv[])
{
    prepare();

    pthread_barrier_t barrier;
    if (run_pth_fam(&pthread_barrier_init, &barrier, nullptr, 2) != 0)
        exit(1);
    pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
    pthread_mutex_t mutex;
    run_pth_fam(&pthread_mutex_init, &mutex, nullptr);

    cond_pair_t cond_pair = std::make_pair(&cond, &mutex);

    std::cout << "Running threads..." << std::endl;
    pthread_attr_t pthread_attr;
    run_pth_fam(&pthread_attr_init, &pthread_attr);
    run_pth_fam(&pthread_attr_setdetachstate, &pthread_attr, PTHREAD_CREATE_DETACHED);
    pthread_t t1;
    run_pth_fam(&pthread_create, &t1, &pthread_attr, &waiting_barrier, &barrier);

    pthread_t t2;
    run_pth_fam(&pthread_create, &t2, nullptr, &waiting_cond, &cond_pair);

    while(!isTerminated)
        pause();

    std::cout << "Waking up threads..." << std::endl;
    if (pthread_barrier_wait(&barrier) == PTHREAD_BARRIER_SERIAL_THREAD)
        std::cout << "PTHREAD_BARRIER_SERIAL_THREAD: " << PTHREAD_BARRIER_SERIAL_THREAD << std::endl;
    run_pth_fam(&pthread_mutex_lock, cond_pair.second);
    run_pth_fam(&pthread_cond_broadcast, &cond);
    std::cout << "unlock0: " << run_pth_fam(&pthread_mutex_unlock, &mutex) << std::endl;

    run_pth_fam(pthread_join, t2, nullptr);
    std::cout << "unlock2: " << run_pth_fam(&pthread_mutex_unlock, &mutex) << std::endl;

    std::cout << "Freeing resources..." << std::endl;
    run_pth_fam(&pthread_barrier_destroy, &barrier);
    run_pth_fam(&pthread_cond_destroy, &cond);
    run_pth_fam(&pthread_mutex_destroy, &mutex);
    run_pth_fam(&pthread_attr_destroy, &pthread_attr);

    std::cout << "Finished" << std::endl;
    return 0;
}

