#include<stdio.h> //printf
#include<string.h> //memset
#include<stdlib.h> //exit(0);
#include<arpa/inet.h>
#include<sys/socket.h>

#define SERVER "127.0.0.1"
#define BUFLEN 512  //Max length of buffer
#define PORT 8888   //The port on which to send data

//mensagem de erro
void die(char *s){
    perror(s);
    exit(1);
}

void main(void){
    struct sockaddr_in si_other;
    int s, i, slen=sizeof(si_other);
    char buf[BUFLEN];
    char message[BUFLEN];
    //create a socket client
    if ( (s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
        die("socket");
    // clear client structure */
    memset((char *) &si_other, 0, sizeof(si_other));
    si_other.sin_family = AF_INET;  //for IPV4 internet protocol
    si_other.sin_port = htons(PORT); //assign port number on which server listening
    if (inet_aton(SERVER , &si_other.sin_addr) == 0){ // assign server machine address
        fprintf(stderr, "inet_aton() failed\n");
        exit(1);
    }
    while(1){
        printf("Enter message : ");
        //gets(message);
        scanf("%s",message);
        //send the message
        if (sendto(s, message, strlen(message) , 0 , (struct sockaddr *) &si_other, slen)==-1)
            die("sendto()");
        //receive a reply and print it
        //clear the buffer by filling null, it might have previously received data
        memset(buf,'\0', BUFLEN);
        //try to receive some data, this is a blocking call
        if (recvfrom(s, buf, BUFLEN, 0, (struct sockaddr *) &si_other, &slen) == -1)
            die("recvfrom()");
        puts(buf);
    }
    close(s);
}
