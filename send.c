#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <fcntl.h>
#include <unistd.h>

#define MAX_MSG_SIZE 1024
#define MSG_KEY_FILE "msg_queue_key_file"
#define MSG_KEY_ID 1234

struct message {
    long msg_type;
    char msg_text[MAX_MSG_SIZE];
};

void send_file(int msgid, const char *file_path) {
    struct message msg;
    FILE *file = fopen(file_path, "r");
    if (file == NULL) {
        perror("File opening failed");
        exit(EXIT_FAILURE);
    }

    msg.msg_type = 1;
    size_t bytes_read;

    // Đọc file và gửi dữ liệu
    while ((bytes_read = fread(msg.msg_text, sizeof(char), MAX_MSG_SIZE, file)) > 0) {
        if (msgsnd(msgid, &msg, bytes_read, 0) == -1) {
            perror("msgsnd");
            exit(EXIT_FAILURE);
        }
    }

    // Gửi một message để thông báo việc gửi file đã hoàn thành
    strcpy(msg.msg_text, "EOF");
    if (msgsnd(msgid, &msg, strlen("EOF") + 1, 0) == -1) {
        perror("msgsnd");
        exit(EXIT_FAILURE);
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

    // Tạo hoặc mở file tạm cho key
    int key_file = open(MSG_KEY_FILE, O_CREAT | O_RDWR, 0666);
    if (key_file == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }
    close(key_file);

    // Tạo key từ file tạm
    key = ftok(MSG_KEY_FILE, MSG_KEY_ID);
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

    // Gửi file được chỉ định trong tham số dòng lệnh
    send_file(msgid, argv[1]);

    // Xóa file tạm
    if (remove(MSG_KEY_FILE) != 0) {
        perror("remove");
        exit(EXIT_FAILURE);
    }

    return 0;
}
