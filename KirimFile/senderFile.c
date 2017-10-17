#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>

struct TextMessage {
    unsigned int seqNum;
    char text[100];
};

int main(void)
{
    int listenfd = 0;
    int connfd = 0;
    struct sockaddr_in serv_addr;
    char sendBuff[1025];
    int numrv;
    struct TextMessage * newMessage = malloc(sizeof(struct TextMessage));
    int messageSeqNum;
    int finished = 0;

    listenfd = socket(AF_INET, SOCK_STREAM, 0);

    printf("Socket retrieve success\n");

    memset(&serv_addr, '0', sizeof(serv_addr));
    memset(sendBuff, '0', sizeof(sendBuff));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(5000);

    bind(listenfd, (struct sockaddr*)&serv_addr,sizeof(serv_addr));

    if(listen(listenfd, 10) == -1)
    {
        printf("Failed to listen\n");
        return -1;
    }


    while(1)
    {
        connfd = accept(listenfd, (struct sockaddr*)NULL ,NULL);

        /* Open the file that we wish to transfer */
        FILE *fp = fopen("sample_file.txt","rb");
        if(fp==NULL)
        {
            printf("File opern error");
            return 1;   
        }   

        /* Read data from file and send it */
        messageSeqNum = 0;
        while(1)
        {   
            /* First read file in chunks of 1 bytes */
            unsigned char buff[1]={0};
            int nread = fread(buff,1,1,fp);
            newMessage->seqNum = messageSeqNum;
            strncpy(newMessage->text, buff, sizeof(buff));
            printf("Bytes read %d, data : %s, segment number : %d \n", nread, newMessage->text, newMessage->seqNum);        
            
            /* If read was success, send data. */
            if(nread > 0)
            {
                printf("Sending \n");
                send(connfd, newMessage, sizeof(newMessage), 0);
            }

            /*
             * There is something tricky going on with read .. 
             * Either there was error, or we reached end of file.
             */
            if (nread < 1)
            {
                if (feof(fp)) 
                    printf("End of file\n");
                if (ferror(fp))
                    printf("Error reading\n");
                finished = 1;
                break;
            }

            messageSeqNum++;
        }
        if (finished) {
            break;
        }        
        close(connfd);
        sleep(1);
    }


    return 0;
}