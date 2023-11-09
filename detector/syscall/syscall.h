struct data_t {
   int pid;
   int uid;
   char command[16];
   char message[12];
   char path[32];
};

struct msg_t {
   char message[12];
};
