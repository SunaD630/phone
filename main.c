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
void ring(){
    FILE *fp;
    char *ring_cmd = "play -b 16 -c 1 -e s -r 88200 南国パラダイス.wav";
    /*if ((fp = popen(ring_cmd, "r")) == NULL){
        perror("poepn failed");
        exit(EXIT_FAILURE);
    }*/
    fp = popen(ring_cmd,"w");
    pclose(fp);
}
int main(){
    ring();
    return 0;
}