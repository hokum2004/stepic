#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>

const char key_path[] = "/tmp/msg.temp";
const int proj_id = 1;
const char output[]="/home/box/message.txt";

struct message {
    long mtype;
    char mtext[80];
};


void check_existing_key_path();
int get_msgq_id();

