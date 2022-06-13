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
    if (argc != 2){
        printf("invalid format\n");
        return 0;
    }
    int port = atoi(argv[1]); //ポート番号
    int N = 256; //読み出しの単位
    int n = 1;
    int w;
    char data[N];
    int ss = socket(PF_INET,SOCK_STREAM,0);
    if (ss==-1){
        printf("Fail to socket\n");
        return 0;
    }
    struct sockaddr_in addr;
    addr.sin_family = AF_INET; //通信体系　IPv4
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);
    bind(ss,(struct sockaddr*)&addr,sizeof(addr));//bindシステムコールにaddrを教える
    listen(ss,10);
    struct sockaddr_in client_addr;
    socklen_t len = sizeof(struct sockaddr_in);
    int s = accept(ss,(struct sockaddr*)&client_addr,&len);
    
    while(n > 0){
        n = read(0,data,N);
        w = send(s,data,n,0);
    }
    
    close(s);
    return 0;
}