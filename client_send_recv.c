#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
int main(int argc, char **argv){
    if (argc != 3){
        printf("invalid format\n");
        return 0;
    }
    char *ip = argv[1]; //ipアドレス
    int port = atoi(argv[2]); //ポート番号
    int N = 256; //読み出しの単位
    int n = 1;
    int w;
    char data[256];
    int s = socket(PF_INET,SOCK_STREAM,0);
    if (s==-1){
        printf("Fail to socket\n");
        return 0;
    }
    struct sockaddr_in addr;
    addr.sin_family = AF_INET; //通信体系　IPv4
    addr.sin_addr.s_addr = inet_addr(ip);
    addr.sin_port = htons(port);
    int ret = connect(s,(struct sockaddr *)&addr, sizeof(addr));
    if (ret == -1){
        printf("Fail to connect\n");
        return 0;
    }
    while(n > 0){
        n = read(0,data,N);
        w = send(s,data,n,0);
    }
    shutdown(s,SHUT_WR);
    n = 1;
    while(n>0){
        n = recv(s,data,N,0);
        w = write(s,data,N);
    }
    close(s);
    return 0;
}