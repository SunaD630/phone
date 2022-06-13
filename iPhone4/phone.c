#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <err.h>
#define N 256

int main(int argc, char **argv) {
    short s_data[N], r_data[N];
    int s;
    int port;

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;

    struct sockaddr_in client_addr;
    socklen_t len = sizeof(struct sockaddr_in);

    if (argc == 2) {
        /*サーバー側*/
        fprintf(stderr, "こちらがサーバーです");
        port = atoi(argv[1]);
        addr.sin_port = htons(port);
        addr.sin_addr.s_addr = INADDR_ANY;

        int ss = socket(PF_INET, SOCK_STREAM, 0);
        fprintf(stderr, "%d\n", ss);

        int b = bind(ss, (struct sockaddr *)&addr, sizeof(addr));
        fprintf(stderr, "%d\n", b);

        int l = listen(ss, 10);
        fprintf(stderr, "%d\n", l);
        
        s = accept(ss, (struct sockaddr *)&client_addr, &len);
        fprintf(stderr, "%d\n", s);
        close(ss);
        
    } else if (argc == 3) {
        /*クライアント側*/
        fprintf(stderr, "こちらがクライアントです");
        char *address = argv[1];
        port = atoi(argv[2]);
        addr.sin_port = htons(port);
        addr.sin_addr.s_addr = inet_addr(address);

        s = socket(PF_INET, SOCK_STREAM, 0);
        fprintf(stderr, "%d\n", s);
        int ret = connect(s, (struct sockaddr *)&addr, sizeof(addr));
        fprintf(stderr, "%d\n", ret);
    } else {
        fprintf(stderr, "error 引数の数が違います");
    }

    FILE *fp = NULL;
    char *cmdline = "rec -t raw -b 16 -c 1 -e s -r 44100 - ";
    if ((fp = popen(cmdline, "r")) == NULL) {
        perror("popen failed");
        exit(EXIT_FAILURE);
    }

    while (1) {
        int sn = fread(s_data, sizeof(char), N*sizeof(short), fp);
        /*fprintf(stderr, "%d\n", sn);*/
        if (sn == -1) {perror("read"); exit(1); }
        if (sn == 0) break;

        int rn = read(s, r_data, N*sizeof(short));
        if (rn == -1) {perror("recv"); exit(1); }
        if (rn == 0) break;

        write(s, s_data, sn);
        write(1, r_data, rn);
    }
    pclose(fp);
    shutdown(s, SHUT_WR);
    return 0;
}