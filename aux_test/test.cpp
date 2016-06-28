#include "msg.h"

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <sys/msg.h>

int main(int argc, char * const argv[])
{
    check_existing_key_path();

    int msgq_id = get_msgq_id();

    message msg;

    msg.mtype = 1313;
    strcpy(msg.mtext, "test data");

    ssize_t was_sent =  msgsnd(msgq_id, &msg, strlen(msg.mtext) + 1, 0);
    if (was_sent == -1) {
        perror("msgrcv");
    }

    return 0;
}
