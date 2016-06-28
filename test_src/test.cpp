#include <iostream>
#include <memory>
#include <cstdio>
#include <cstdlib>

#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <unistd.h>

int main(int argc, char * const argv[])
{
    const char queue_name[] = "/test.mq";

    mqd_t mqdes = mq_open(queue_name, O_WRONLY);
    if (mqdes == -1) {
        perror("mq_open");
        exit(1);
    }

    mq_attr attr;
    if (mq_getattr(mqdes, &attr) == -1) {
        perror("mq_getattr");
        exit(2);
    }

    std::cout << "flags: " << attr.mq_flags << '\n'
              << "maxmsg: " << attr.mq_maxmsg << '\n'
              << "msgsize: " << attr.mq_msgsize << std::endl;

    char message[] = "Hello, world!";

    ssize_t was_sent = mq_send(mqdes, message, sizeof(message), 13);
    if (was_sent == -1) {
        perror("mq_send");
        exit(3);
    }

    if (mq_close(mqdes) == -1)
        perror("mq_close");

    return 0;
}

