#define MAX_ARGS 5
#define ARG_LEN 64

struct data_t {
   int pid;
   int uid;
   char command[16];
   char message[12];
   char path[32];
   char argcv[MAX_ARGS][ARG_LEN];
};

struct msg_t {
   char message[12];
};
