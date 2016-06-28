#include "msg.h"

#include <cstdio>
#include <cstdlib>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

void check_existing_key_path()
{
    if (access(key_path, X_OK) == -1) {
        int file = creat(key_path, 0666);
        if (file == -1) {
            perror("Create key file.");
            exit(1);
        }
        close(file);
    }
}

int get_msgq_id()
{
    key_t msgq_key = ftok(key_path, proj_id);
    if (msgq_key == -1) {
        perror("ftok");
        exit(2);
    }

    int msgq_id = msgget(msgq_key, IPC_CREAT | 0666);
    if (msgq_id == -1) {
        perror("msgget");
        exit(3);
    }

    return msgq_id;
}

