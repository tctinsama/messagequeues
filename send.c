#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <ncurses.h>

#define MAX_TEXT 512 // Kích thước tối đa của tin nhắn

struct my_msg {
    long int msg_type;
    char file_data[MAX_TEXT];
};

int main() {
    int running = 1;
    int msgid;
    struct my_msg some_data;
    char filename[MAX_TEXT];

    initscr(); // Khởi tạo ncurses
    printw("Enter the filename to send: ");
    refresh();
    getstr(filename); // Nhập tên file

    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printw("Error in opening file\n");
        exit(EXIT_FAILURE);
    }

    msgid = msgget((key_t)14534, 0666 | IPC_CREAT); // Tạo hoặc kết nối hàng đợi tin nhắn
    if (msgid == -1) {
        printw("Error in creating queue\n");
        exit(EXIT_FAILURE);
    }

    while (running) {
        if (fgets(some_data.file_data, MAX_TEXT, file) == NULL) {
            strcpy(some_data.file_data, "end");
            running = 0;
        }

        some_data.msg_type = 1;

        if (msgsnd(msgid, (void *)&some_data, MAX_TEXT, 0) == -1) {
            printw("Msg not sent\n");
            exit(EXIT_FAILURE);
        }

        usleep(1000);
    }

    fclose(file);
    endwin(); // Kết thúc ncurses
    exit(EXIT_SUCCESS);
}
