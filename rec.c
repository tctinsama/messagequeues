#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define MAX_MSG_SIZE 1024
#define MSG_KEY 1234

struct message {
    long msg_type;
    char msg_text[MAX_MSG_SIZE];
};

void receive_file(int msgid, const char *file_path) {
    struct message msg;
    FILE *file = fopen(file_path, "w");
    if (file == NULL) {
        perror("File opening failed");
        exit(EXIT_FAILURE);
    }

    size_t bytes_received;

    // Nhận dữ liệu từ message queue và ghi vào file
    while (1) {
        if (msgrcv(msgid, &msg, sizeof(msg.msg_text), 1, 0) == -1) {
            perror("msgrcv");
            exit(EXIT_FAILURE);
        }

        if (strcmp(msg.msg_text, "EOF") == 0) {
            break; // Kết thúc việc nhận file khi nhận được tin nhắn "EOF"
        }

        bytes_received = strlen(msg.msg_text);
        fwrite(msg.msg_text, sizeof(char), bytes_received, file);
    }

    fclose(file);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <file_path>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int msgid;
    key_t key;

    // Tạo key cho message queue
    key = ftok("msg_queue_key", MSG_KEY);
    if (key == -1) {
        perror("ftok");
        exit(EXIT_FAILURE);
    }

    // Tạo hoặc kết nối vào message queue
    msgid = msgget(key, 0666 | IPC_CREAT);
    if (msgid == -1) {
        perror("msgget");
        exit(EXIT_FAILURE);
    }

    // Nhận file được chỉ định trong tham số dòng lệnh
    receive_file(msgid, argv[1]);

    return 0;
}
