

/* Sample UDP server */

#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char**argv)
{
    int sockfd, n, i;
    struct sockaddr_in servaddr, cliaddr;
    socklen_t len;
    char mesg[1024];
    unsigned int startseq = 0, seq   = 0;
    int          start = 1;
    int          wfd;
    unsigned int wlen  = 0;

    wfd = open("./udpsave", O_RDWR | O_CREAT);

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    bzero(&servaddr,sizeof(servaddr));
    servaddr.sin_family       = AF_INET;
    servaddr.sin_addr.s_addr  = htonl(INADDR_ANY);
    servaddr.sin_port         = htons(1234);
    bind(sockfd,(struct sockaddr *)&servaddr,sizeof(servaddr));

    i = 0;
    for (;;){

        memset(mesg, 0, sizeof mesg);

        len = sizeof(cliaddr);
        n   = recvfrom(sockfd, mesg, 1024, 0, (struct sockaddr *)&cliaddr, &len);
        if(n <= 0){
            printf("UDP receive error! \n");
                break;
        }

        n = write(wfd, mesg, n);
        if(n <= 0){
            printf("Write to local failed \n");
            break;
        }

        wlen += n;

        i++;

        seq = *((unsigned int *)&mesg[0]);
        if(start == 1){
           start = 0;
           startseq = seq;
        }


        //printf("receive %d bytes i %d seq: %d : %d \n", n, i, seq, i % 100);
        if( (i % 100) == 0){
            printf("UDP receive %d%% \r", i * 100 / (seq - startseq + 1));
            fflush(stdout);
        }

        if(wlen >= 1 * 1024 * 1024){
            break;
        }

    }

    fsync(wfd);

    return 0;
}
