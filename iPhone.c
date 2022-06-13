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
#include <string.h>
#include <pthread.h>
struct comm_data{
    int s;//acceptの返り値
    char *data;//受け渡しするときの配列
    FILE *fp;
};
void rec_send(void *arg){
    struct comm_data *pd = (struct comm_data *)arg;
    char *data = pd->data;
    int s = pd->s;
    char *rec_cmd = "rec -t raw -b 16 -c 1 -e s -r 44100 -";
    FILE *fp;
    int N = 256;
    int w;
    int n;
     if ((fp = popen(rec_cmd, "r")) == NULL){
        perror("poepn failed");
        exit(EXIT_FAILURE);
    }
    while(1){
        w = fread(data,sizeof(char),N,fp);
        n = send(s,data,w,0);
        //printf("sending\n");
    }
    pclose(fp);
}
void ring(void *arg){
    struct comm_data *pd = (struct comm_data *)arg;
    FILE *fp = pd->fp;
    int s = pd->s;
    char *ring_cmd = "play -b 16 -c 1 -e s -r 88200 南国パラダイス.wav";
    fp = popen(ring_cmd, "r");
    while(1){
        if (s == 1){
            pclose(fp);
            break;
        }
    }
}
void play_recv(void *arg){
    struct comm_data *pd = (struct comm_data *)arg;
    char *data = pd->data;
    int s = pd->s;
    FILE *fp = pd->fp;
    char *play_cmd = "play -t raw -b 16 -c 1 -e s -r 44100 -";
    int n = 1;
    int N = 256;
    int w;
    if ((fp = popen(play_cmd, "w")) == NULL){
        perror("poepn failed");
        exit(EXIT_FAILURE);
    }
    while(1){
        n = recv(s,data,N,0);
        w = fwrite(data,sizeof(char),n,fp);
        //printf("receive\n");
    }
    pclose(fp);
}
void server(int port){
    int N = 256; //読み出しの単位
    char data1[N];
    char data2[N];
    FILE *fp;
    //char *cmdline = "rec -t raw -b 16 -c 1 -e s -r 44100 - ";
    int ss = socket(PF_INET,SOCK_STREAM,0);
    if (ss==-1){
        perror("Fail to socket\n");
        exit(EXIT_FAILURE);
    }
    struct sockaddr_in addr;
    addr.sin_family = AF_INET; //通信体系　IPv4
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);
    bind(ss,(struct sockaddr*)&addr,sizeof(addr));//bindシステムコールにaddrを教える
    listen(ss,10);
    struct sockaddr_in client_addr;
    socklen_t len = sizeof(struct sockaddr_in);
    struct comm_data d[3];
    pthread_t t[3];
    d[2].fp = fp;
    d[2].s = 0;
    //pthread_create(&t[2],NULL,(void*)ring,&d[2]);
    int s = accept(ss,(struct sockaddr*)&client_addr,&len);
    d[2].s = 1;
    d[0].s = s;
    d[0].data = data1;
    d[1].s = s;
    d[1].data = data2;
    pthread_create(&t[0],NULL,(void*)rec_send,&d[0]);
    pthread_create(&t[1],NULL,(void*)play_recv,&d[1]);
    printf("s is %d\n",s);
    pthread_join(t[0],NULL);
    pthread_join(t[1],NULL);
    pthread_join(t[2],NULL);
    close(ss);
    close(s);
}

void client(char *ip,int port){
    int N = 256; //読み出しの単位
    int n = 1;
    int w;
    char data1[N];
    char data2[N];
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

    struct comm_data d[2];
    pthread_t t[2];
    d[0].s = s;
    d[0].data = data1;
    d[1].s = s;
    d[1].data = data2;

    pthread_create(&t[0],NULL,(void*)rec_send,&d[0]);
    pthread_create(&t[1],NULL,(void*)play_recv,&d[1]);
    pthread_join(t[0],NULL);
    pthread_join(t[1],NULL);
    close(s);
}

int main(int argc, char **argv){
    if (argc == 2){
        int port = atoi(argv[1]);
        server(port);
    }else if(argc == 3){
        char *ip = argv[1];
        int port = atoi(argv[2]);
        client(ip,port);
    }
    
    return 0;
}