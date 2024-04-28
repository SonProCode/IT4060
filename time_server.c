#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <time.h>
void signalHanlder(int signo){
    pid_t pid = wait(NULL);
}


int main(){
    int listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listener == -1){
        perror("socket() failed");
        return 1;
    }

    // khai bao dia chi server
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(8001);

    // Gan socket voi cau truc dia chi
    if (bind(listener, (struct sockaddr *)&addr, sizeof(addr))) {
        perror("bind() failed");
        return 1;
    }

    // Chuyen socket sang trang thai cho ket noi
    if (listen(listener, 5)) {
        perror("listen() failed");
        return 1;
    }

    signal(SIGCHLD, signalHanlder);

    while(1){
        printf("Waitng for new client....\n");
        int client = accept(listener, NULL, NULL);
        printf("New client accepted, client = %d\n", client);

        if (fork() == 0){
            // tien trinh xu li yeu cau cua client
            // dong socket listener
            close(listener);

            char buf[256];
            while (1){
                int ret = recv(client, buf, sizeof(buf), 0);
                if (ret <= 0)
                    break;
                
                buf[ret] = 0;
                printf("Xau nhan duoc: %s", buf);
                char todo[15];
                char format[20];
                char abun[20];
                int n = sscanf(buf, "%s %s %s", todo, format, abun);
                
                if (n != 2 || strcmp(todo, "GET_TIME") != 0 || 
                    (strcmp(format, "dd/mm/yyyy") != 0 &&
                    strcmp(format, "dd/mm/yy") != 0 &&
                    strcmp(format, "mm/dd/yyyy") != 0 &&
                    strcmp(format, "mm/dd/yy") != 0)
                ){
                    send(client, "sai format\n", sizeof("sai format\n"), 0);
                    continue;
                }
                //send(client, "working", sizeof("working"), 0);
                time_t t = time(NULL);
                char time_str[20];
                if (strcmp(format, "dd/mm/yyyy") == 0)
                    strftime(time_str, sizeof(time_str), "%d/%m/%Y\n", localtime(&t));
                if (strcmp(format, "dd/mm/yy") == 0)
                    strftime(time_str, sizeof(time_str), "%d/%m/%y\n", localtime(&t));
                if (strcmp(format, "mm/dd/yyyy") == 0)
                    strftime(time_str, sizeof(time_str), "%m/%d/%Y\n", localtime(&t));
                if (strcmp(format, "mm/dd/yy") == 0)
                    strftime(time_str, sizeof(time_str), "%m/%d/%y\n", localtime(&t));
                send(client, time_str, sizeof(time_str), 0);
            }
            close(client);
            exit(0);
        }

        close(client);

    }

}
