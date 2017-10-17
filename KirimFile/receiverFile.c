#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>


struct TextMessage {
    unsigned int seqNum;
    char text[100];
};


int main(void)
{
    int sockfd = 0;
    int bytesReceived = 0;
    char recvBuff[1];
    memset(recvBuff, '0', sizeof(recvBuff));
    struct sockaddr_in serv_addr;
    struct TextMessage * newMessage = malloc(sizeof(struct TextMessage));
    int messageSeqNum;

    /* Create a socket first */
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0))< 0)
    {
        printf("\n Error : Could not create socket \n");
        return 1;
    }

    /* Initialize sockaddr_in data structure */
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(5000); // port
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    /* Attempt a connection */
    if(connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr))<0)
    {
        printf("\n Error : Connect Failed \n");
        return 1;
    }

    /* Create file where data will be stored */
    FILE *fp;
    fp = fopen("received.txt", "w"); 
    if(NULL == fp)
    {
        printf("Error opening file");
        return 1;
    }

    /* Receive data in chunks of 1 bytes */
    while((bytesReceived = read(sockfd, newMessage, sizeof(*newMessage))) > 0)
    {
        strncpy(recvBuff, newMessage->text, sizeof(recvBuff));
        printf("Segment number : %d", newMessage->seqNum);
        printf("Bytes received %d\n",bytesReceived);    
        printf("Data received : %s\n", newMessage->text);
        fwrite(recvBuff, 1,sizeof(recvBuff),fp);
    }

    if(bytesReceived < 0)
    {
        printf("\n Read Error \n");
    }


    return 0;
}