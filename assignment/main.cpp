#include "msg.h"

#include <iostream>
#include <cstdio>
#include <cstdlib>

#include <sys/msg.h>

int main(int argc, char * const argv[])
{
    check_existing_key_path();

    int msgq_id = get_msgq_id();

    int file = creat(output, 0666);
    if (file == -1) {
        perror("creat");
        exit(10);
    }

    message msg;

    ssize_t was_received =  msgrcv(msgq_id, &msg, sizeof(message) - sizeof(message::mtype), 0, 0);
    if (was_received == -1) {
        perror("msgrcv");
    }
    else {
        std::cout << "Message with type " << msg.mtype << " was received." << std::endl;
        ssize_t was_written = write(file, msg.mtext, was_received);
        if (was_written == -1)
            perror("write");
    }

    close(file);

    return 0;
}

