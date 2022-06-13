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
#include <netinet/udp.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
int main(int argc, char **argv){
    if (argc != 3){
        printf("invalid format\n");
        return 0;
    }
    char *ip = argv[1]; //ipアドレス
    int port = atoi(argv[2]); //ポート番号
    int N = 1000; //読み出しの単位
    int n = 1;
    int w = 1;
    char a = 68;
    char data[256];
    int s = socket(PF_INET,SOCK_DGRAM,0); //UDP通信
    if (s==-1){
        printf("Fail to socket\n");
        return 0;
    }
    struct sockaddr_in addr;
    addr.sin_family = AF_INET; //通信体系　IPv4
    addr.sin_addr.s_addr = inet_addr(ip);
    addr.sin_port = htons(port);
    socklen_t  addrlen;
    n = sendto(s,&a,1,0,(struct sockaddr *)&addr,sizeof(addr));
    while(n>0){
        n = recvfrom(s,data,N,0,(struct sockaddr *)&addr,&addrlen);
        int i;
        for (i = 0; i < n ; i++){
            if (data[i] == 1){
                continue;
            }else{
                break;
            }
        }
        if (i == 999){
            break;
        }
        w = write(1,data,n);
    }
    close(s);
    return 0;
}