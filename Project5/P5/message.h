#define TEXT_SIZE 256

struct shared_use_st {
    int written_by_you;
    char data[TEXT_SIZE];
};

typedef struct shared_use_st shared_use_st;