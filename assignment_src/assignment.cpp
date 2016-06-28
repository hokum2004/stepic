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
    const char output_file[] = "/home/box/message.txt";

    mqd_t mqdes = mq_open(queue_name, O_RDONLY | O_CREAT | O_EXCL, 0666, nullptr);
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

    std::unique_ptr<char[]> message {new char[attr.mq_msgsize]};
    unsigned int prio;

    ssize_t was_received = mq_receive(mqdes, message.get(), attr.mq_msgsize, &prio);
    if (was_received == -1) {
        perror("mq_received");
        exit(3);
    }
    else {
        std::cout << "was received " << was_received << " bytes." << std::endl;
        std::cout << "message's priority is " << prio << "." << std::endl;

        int file = creat(output_file, 0666);
        if (file == -1) {
            perror("creat");
            exit(4);
        }

        ssize_t was_written = write(file, message.get(), was_received);
        if (was_written == -1) {
            perror("write");
            exit(5);
        }

        close(file);
    }

    if (mq_unlink(queue_name) == -1)
        perror("mq_unlink");

    if (mq_close(mqdes) == -1)
        perror("mq_close");

    return 0;
}

