#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h> 
int main(int argc, char ** argv){
    //int n;
    unsigned char ones[1000];
    for(int i=0;i<1000;i++){
        ones[i] = '1';
    }
    write(1,ones,1000);
return 0;
}
