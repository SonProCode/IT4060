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
    addr.sin_port = htons(8002);

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
    char buf[2048];
    while(1){
        printf("Waitng for new client....\n");
        int client = accept(listener, NULL, NULL);
        if (client == -1) continue;
        printf("New client accepted, client = %d\n", client);
        if (fork() == 0){ 
            close(listener);
            int ret = recv(client, buf, sizeof(buf), 0);
            if (ret <= 0) {
                close(client);
                continue;
            }

            if (ret < sizeof(buf))
                buf[ret] = 0;
            printf("%s\n", buf);
            close(client);
            exit(0);
        }
        else {
             char msg[] = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<html><body><h1>Hello World</h1></body></html>\n";
             send(client, msg, strlen(msg), 0);
        }
        close(client);
    }

}