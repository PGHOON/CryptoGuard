struct data_t {
   int pid;
   int uid;
   char command[32];
   char message[32];
   char path[32];
};

struct msg_t {
   char message[12];
};
