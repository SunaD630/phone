#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>

#define SIZE 256
#define SAMP 44100

void buff_clear(time_t start) {
    time_t end = time(NULL);
    int num = ((int)(end - start)) * SAMP * sizeof(short);
    int sum = 0;
    short data[SAMP];
    while(1) {
        int n = read(0, data, sizeof(short) * SAMP);
        sum += n;
        if(sum >= num) break;
    }
}

void* rec(void* arg) {
    int n;
    int s = *(int *)arg;
    short data[SIZE];
    while(1) {
        n = read(0, data, SIZE * sizeof(short));
        //fprintf(stderr, "%d\n", n);
        if(n == -1) {perror("read"); exit(1);}
        write(s, data, n);
    }    
}

void* play(void* arg) {
    int m;
    short data[SIZE];
    int s = *(int *)arg;
    while(1) {
        m = read(s, data, SIZE * sizeof(short));
        //fprintf(stderr, "%d\n", m);
        if(m == -1) {perror("receive"); exit(1);}
         write(1, data, m);
    }
}

int client_num = 1;
short voice[4][SIZE];
void* serve(void* arg) {
    int n;
    int s = *(int *)arg;
    short data[SIZE];
    short send[SIZE];
    while(1) {
        n = read(0, voice[0], SIZE * sizeof(short));
        if(n == -1) {perror("read"); exit(1);}
        for(int i = 1;i <= client_num;i++) {
            n = read(s, voice[i], SIZE * sizeof(short));
            //fprintf(stderr, "%d\n", n);
            if(n == -1) {perror("read"); exit(1);}
        }
        for(int i = 0;i < SIZE;i++) {
            int y = 0;
            for(int j = 0;j <= client_num;j++) {
                y += voice[j][i];
            }
            data[i] = y;
        }
        for(int i = 0;i <= client_num;i++) {
            for(int j = 0;j < SIZE;j++) {
                send[j] = data[j]-voice[i][j];
            }
            if(i > 0) write(s, send, SIZE * sizeof(short));
            else write(1, send, SIZE * sizeof(short));
        }
    }
}

void server(int port, time_t start) {
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

    pthread_t serve_thread;
    pthread_create(&serve_thread, NULL, serve, &s);
    pthread_join(serve_thread, NULL);
}

void client(int port, char *IP, time_t start) {
    int s = socket(PF_INET, SOCK_STREAM, 0);
    if(s == -1) {perror("socket"); exit(1);}

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(IP);
    addr.sin_port = htons(port);
    int ret = connect(s, (struct sockaddr *)&addr, sizeof(addr));
    if(ret == -1) {perror("connect"); exit(1);}

    pthread_t rec_thread;
    pthread_t play_thread;
    
    pthread_create(&rec_thread, NULL, rec, &s);
    pthread_create(&play_thread, NULL, play, &s);

    pthread_join(rec_thread, NULL);
    pthread_join(play_thread, NULL);
}

int main(int argc, char *argv[]) {
    time_t start;
    start = time(NULL);
    
    if(argc < 2) {perror("open"); exit(1);}
    if(argc == 2) {
        int port = atoi(argv[1]);
        server(port, start);
    } else {
        char *IP = argv[1];
        int port = atoi(argv[2]);
        client(port, IP, start);
    }
    return 0;
}