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
    printw("Enter the filename to save the received file: ");
    refresh();
    getstr(filename); // Nhập tên file

    FILE *file = fopen(filename, "w");
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
        if (msgrcv(msgid, (void *)&some_data, MAX_TEXT, 1, 0) == -1) {
            printw("Error in receiving message\n");
            exit(EXIT_FAILURE);
        }

        fprintf(file, "%s", some_data.file_data);

        if (strncmp(some_data.file_data, "end", 3) == 0) {
            running = 0;
        }
    }

    fclose(file);

    if (msgctl(msgid, IPC_RMID, 0) == -1) {
        printw("Error in removing queue\n");
        exit(EXIT_FAILURE);
    }

    endwin(); // Kết thúc ncurses
    exit(EXIT_SUCCESS);
}
