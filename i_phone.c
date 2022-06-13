#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>

#define SIZE 1024
#define SAMP 44100

void buff_clear(time_t start) {
    time_t end = time(NULL);
    int num = ((int)(end - start)) * SAMP * sizeof(short);
    int sum = 0;
    short data[SAMP];
    while(1) {
        int n = read(0, data, sizeof(short) * SAMP);
        sum += n;
        //printf("%d %d\n", n, sum);
        if(sum >= num) break;
    }
}

int main(int argc, char *argv[]) {
    time_t start;
    start = time(NULL);
    
    if(argc < 2) {perror("open"); exit(1);}
    if(argc == 2) {
        int port = atoi(argv[1]);
        int ss = socket(PF_INET, SOCK_STREAM, 0);
        if(ss == -1) {perror("socket"); exit(1);}

        struct sockaddr_in addr;
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = INADDR_ANY;
        addr.sin_port = htons(port);
        int b = bind(ss, (struct sockaddr *)&addr, sizeof(addr));
        if(b == -1) {perror("bind"); exit(1);}

        int l = listen(ss, 10);
        if(l == -1) {perror("listen"); exit(1);}

        struct sockaddr_in client_addr;
        socklen_t len = sizeof(struct sockaddr_in);
        int s = accept(ss, (struct sockaddr *)&client_addr, &len);
        if(s == -1) {perror("accept"); exit(1);}

        close(ss);
        buff_clear(start);

        int n, m;
        short data[SIZE];
        while(1) {
            n = read(0, data, SIZE * sizeof(short));
            if(n == -1) {perror("read"); exit(1);}
            if(n == 0) break;
            write(s, data, n);

            m = read(s, data, SIZE * sizeof(short));
            if(m == -1) {perror("receive"); exit(1);}
            if(m == 0) break;
            write(1, data, m);
        }
        close(s);
    } else {
        char *IP = argv[1];
        int port = atoi(argv[2]);
        
        int s = socket(PF_INET, SOCK_STREAM, 0);
        if(s == -1) {perror("socket"); exit(1);}

        struct sockaddr_in addr;
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = inet_addr(IP);
        addr.sin_port = htons(port);
        int ret = connect(s, (struct sockaddr *)&addr, sizeof(addr));
        if(ret == -1) {perror("connect"); exit(1);}

        int n, m;
        short data[SIZE];
        while(1) {
            n = read(0, data, SIZE * sizeof(short));
            if(n == -1) {perror("read"); exit(1);}
            if(n == 0) break;
            write(s, data, n);

            m = read(s, data, SIZE * sizeof(short));
            if(m == -1) {perror("receive"); exit(1);}
            if(m == 0) break;
            write(1, data, m);
        }
        close(s);
    }
    return 0;
}